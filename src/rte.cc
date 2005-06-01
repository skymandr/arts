/* Copyright (C) 2002 Patrick Eriksson <Patrick.Eriksson@rss.chalmers.se>
                      Stefan Buehler   <sbuehler@uni-bremen.de>
                            
   This program is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the
   Free Software Foundation; either version 2, or (at your option) any
   later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
   USA. */



/*===========================================================================
  === File description 
  ===========================================================================*/

/*!
  \file   rte.cc
  \author Patrick Eriksson <Patrick.Eriksson@rss.chalmers.se>
  \date   2002-05-29

  \brief  Functions to solve radiative transfer tasks.
*/



/*===========================================================================
  === External declarations
  ===========================================================================*/

#include <cmath>
#include <stdexcept>
#include "auto_md.h"
#include "check_input.h"
#include "logic.h"
#include "math_funcs.h"
#include "physics_funcs.h"
#include "ppath.h"
#include "rte.h"
#include "special_interp.h"
#include "lin_alg.h"




/*===========================================================================
  === The functions in alphabetical order
  ===========================================================================*/

//! get_radiative_background
/*!
    Sets *iy* to the radiative background for a propagation path.

    The function uses *ppath* to determine the radiative background
    for a propagation path and calls the relevant agenda. Coding of
    backgrounds is described in the header of the function
    ppath_set_background (in ppath.cc).

    The main purpose of the function is set *iy*. It is NOT needed to set 
    *iy* to the correct size before calling the function.

    \param   iy                 Output: As the WSV with the same name.
    \param   rte_pos            Output: As the WSV with the same name.
    \param   rte_gp_p           Output: As the WSV with the same name.
    \param   rte_gp_lat         Output: As the WSV with the same name.
    \param   rte_gp_lon         Output: As the WSV with the same name.
    \param   rte_los            Output: As the WSV with the same name.
    \param   ppath              Input: As the WSV with the same name.
    \param   iy_space_agenda    Input: As the WSV with the same name.
    \param   iy_surface_agenda  Input: As the WSV with the same name.
    \param   iy_cloudbox_agenda Input: As the WSV with the same name.
    \param   f_grid             Input: As the WSV with the same name.
    \param   stokes_dim         Input: As the WSV with the same name.
    \param   agenda_verb        Input: Argument handed to agendas to control
                                verbosity.

    \author Patrick Eriksson 
    \date   2002-09-17
*/
void get_radiative_background(
              Matrix&         iy,
              Vector&         rte_pos,
              GridPos&        rte_gp_p,
              GridPos&        rte_gp_lat,
              GridPos&        rte_gp_lon,
              Vector&         rte_los,
              Ppath&          ppath,
        const Agenda&         iy_space_agenda,
        const Agenda&         iy_surface_agenda,
        const Agenda&         iy_cloudbox_agenda,
        const Index&          atmosphere_dim,
        ConstVectorView       f_grid,
        const Index&          stokes_dim,
        const Index&          agenda_verb )
{
  // Some sizes
  const Index nf      = f_grid.nelem();
  const Index np      = ppath.np;

  // Set rte_pos, rte_gp_XXX and rte_los to match the last point in ppath.
  // The agendas below use different combinations of these variables.
  //
  // Note that the Ppath positions (ppath.pos) for 1D have one column more
  // than expected by most functions. Only the first atmosphere_dim values
  // shall be copied.
  //
  rte_pos.resize( atmosphere_dim );
  rte_pos = ppath.pos(np-1,Range(0,atmosphere_dim));
  rte_los.resize( ppath.los.ncols() );
  rte_los = ppath.los(np-1,joker);
  gridpos_copy( rte_gp_p, ppath.gp_p[np-1] );
  if( atmosphere_dim > 1 )
    { gridpos_copy( rte_gp_lat, ppath.gp_lat[np-1] ); }
  if( atmosphere_dim > 2 )
    { gridpos_copy( rte_gp_lon, ppath.gp_lon[np-1] ); }

  out3 << "Radiative background: " << ppath.background << "\n";


  // Initialize iy to the radiative background
  switch ( ppath_what_background( ppath ) )
    {

    case 1:   //--- Space ---------------------------------------------------- 
      {
        chk_not_empty( "iy_space_agenda", iy_space_agenda );
        iy_space_agenda.execute( agenda_verb );
     

        if( iy.nrows() != nf  ||  iy.ncols() != stokes_dim )
          {
            out1 << "expected size = [" << nf << "," << stokes_dim << "]\n";
            out1 << "iy size = [" << iy.nrows() << "," << iy.ncols()<< "]\n";
            throw runtime_error( "The size of *iy* returned from "
                                          "*iy_space_agenda* is not correct.");
          }
      }
      break;


    case 2:   //--- The surface -----------------------------------------------
      {
        // Make a copy of *ppath* as the WSV will be changed below, but the
        // original data is needed when going back to *rte_calc*.
        Ppath   pp_copy;
        ppath_init_structure( pp_copy, atmosphere_dim, ppath.np );
        ppath_copy( pp_copy, ppath );

        chk_not_empty( "iy_surface_agenda", iy_surface_agenda );
        iy_surface_agenda.execute( agenda_verb );

        if( iy.nrows() != nf  ||  iy.ncols() != stokes_dim )
          {
            out1 << "expected size = [" << nf << "," << stokes_dim << "]\n";
            out1 << "iy size = [" << iy.nrows() << "," << iy.ncols()<< "]\n";
            throw runtime_error( "The size of *iy* returned from "
                                        "*iy_surface_agenda* is not correct.");
          }

        // Copy data back to *ppath*.
        ppath_init_structure( ppath, atmosphere_dim, pp_copy.np );
        ppath_copy( ppath, pp_copy );
      }
      break;


    case 3:   //--- Cloudbox surface -----------------------------------------
      {
        // Make a copy of *ppath* as the WSV will be changed below, but the
        // original data is needed when going back to *rte_calc*.
        Ppath   pp_copy;
        ppath_init_structure( pp_copy, atmosphere_dim, ppath.np );
        ppath_copy( pp_copy, ppath );
        chk_not_empty( "iy_surface_agenda", iy_surface_agenda );

        chk_not_empty( "iy_cloudbox_agenda", iy_cloudbox_agenda );
        iy_cloudbox_agenda.execute( agenda_verb );

        if( iy.nrows() != nf  ||  iy.ncols() != stokes_dim )
          {
            out1 << "expected size = [" << nf << "," << stokes_dim << "]\n";
            out1 << "iy size = [" << iy.nrows() << "," << iy.ncols()<< "]\n";
            throw runtime_error( "The size of *iy* returned from "
                                      "*iy_cloudbox_agenda* is not correct." );
          }

        // Copy data back to *ppath*.
        ppath_init_structure( ppath, atmosphere_dim, pp_copy.np );
        ppath_copy( ppath, pp_copy );
      }
      break;

    case 4:   //--- Inside of cloudbox ---------------------------------------
      {
        throw runtime_error( "RTE calculations starting inside the cloud box "
                                                       "are not yet handled" );
      }
      break;


    default:  //--- ????? ----------------------------------------------------
      // Are we here, the coding is wrong somewhere
      assert( false );
    }
}



//! iy_calc
/*!
    Solves the monochromatic pencil beam RTE.

    The function performs three basic tasks: <br>
      1. Determines the propagation path (by call of ppath_calc). <br>
      2. Determines the radiative background. <br>
      3. Executes *rte_agenda*. <br> <br>

    The start position and LOS shall be put into *los* and *pos* (and
    not *rte_pos* and *rte_los*).

    It is NOT needed to set *iy* to the correct size before calling
    the function.

    \param   iy                 Output: As the WSV with the same name.
    \param   ppath              Output: As the WSV with the same name.
    \param   ppath_step         Output: As the WSV with the same name.
    \param   rte_pos            Output: As the WSV with the same name.
    \param   rte_gp_p           Output: As the WSV with the same name.
    \param   rte_gp_lat         Output: As the WSV with the same name.
    \param   rte_gp_lon         Output: As the WSV with the same name.
    \param   rte_los            Output: As the WSV with the same name.
    \param   ppath_step_agenda  Input: As the WSV with the same name.
    \param   rte_agenda         Input: As the WSV with the same name.
    \param   iy_space_agenda    Input: As the WSV with the same name.
    \param   iy_surface_agenda  Input: As the WSV with the same name.
    \param   iy_cloudbox_agenda Input: As the WSV with the same name.
    \param   atmosphere_dim     Input: As the WSV with the same name.
    \param   p_grid             Input: As the WSV with the same name.
    \param   lat_grid           Input: As the WSV with the same name.
    \param   lon_grid           Input: As the WSV with the same name.
    \param   z_field            Input: As the WSV with the same name.
    \param   r_geoid            Input: As the WSV with the same name.
    \param   z_surface          Input: As the WSV with the same name.
    \param   cloudbox_on        Input: As the WSV with the same name.
    \param   cloudbox_limits    Input: As the WSV with the same name.
    \param   pos                Input: Observation position.
    \param   los                Input: Observation LOS.
    \param   f_grid             Input: As the WSV with the same name.
    \param   stokes_dim         Input: As the WSV with the same name.
    \param   antenna_dim        Input: As the WSV with the same name.
    \param   agenda_verb        Input: Argument handed to agendas to control
                                verbosity.

    \author Patrick Eriksson 
    \date   2002-09-17
*/
void iy_calc(
              Matrix&         iy,
              Ppath&          ppath,
              Ppath&          ppath_step,
              Vector&         ppath_p,
              Vector&         ppath_t,
              Matrix&         ppath_vmr,
              Vector&         rte_pos,
              GridPos&        rte_gp_p,
              GridPos&        rte_gp_lat,
              GridPos&        rte_gp_lon,
              Vector&         rte_los,
        const Agenda&         ppath_step_agenda,
        const Agenda&         rte_agenda,
        const Agenda&         iy_space_agenda,
        const Agenda&         iy_surface_agenda,
        const Agenda&         iy_cloudbox_agenda,
        const Index&          atmosphere_dim,
        const Vector&         p_grid,
        const Vector&         lat_grid,
        const Vector&         lon_grid,
        const Tensor3&        z_field,
        const Tensor3&        t_field,
        const Tensor4&        vmr_field,
        const Matrix&         r_geoid,
        const Matrix&         z_surface,
        const Index&          cloudbox_on, 
        const ArrayOfIndex&   cloudbox_limits,
        const Vector&         pos,
        const Vector&         los,
        const Vector&         f_grid,
        const Index&          stokes_dim,
        const bool&           agenda_verb )
{
  // Determine propagation path
  const bool   outside_cloudbox = true;
  ppath_calc( ppath, ppath_step, ppath_step_agenda, atmosphere_dim, p_grid, 
              lat_grid, lon_grid, z_field, r_geoid, z_surface,
              cloudbox_on, cloudbox_limits, pos, los, outside_cloudbox );

  // Determine the radiative background
  //
  iy.resize(f_grid.nelem(),stokes_dim);
  //
  get_radiative_background( iy, rte_pos, rte_gp_p, rte_gp_lat, rte_gp_lon,
       rte_los, ppath, iy_space_agenda, iy_surface_agenda, iy_cloudbox_agenda, 
       atmosphere_dim, f_grid, stokes_dim, agenda_verb );

  const Index   np = ppath.np;

  // If the number of propagation path points is 0 or 1, we are already ready,
  // the observed spectrum equals then the radiative background.
  if( np > 1 )
    {
      //- Determine atmospheric fields at each ppath point --------------------
      //
      //- Pressure:
      ppath_p.resize(np);
      Matrix itw_p(np,2);
      interpweights( itw_p, ppath.gp_p );      
      itw2p( ppath_p, p_grid, ppath.gp_p, itw_p );
      //
      //- Temperature:
      ppath_t.resize(np);
      Matrix   itw_field;
      interp_atmfield_gp2itw( itw_field, atmosphere_dim, p_grid, lat_grid, 
                            lon_grid, ppath.gp_p, ppath.gp_lat, ppath.gp_lon );
      interp_atmfield_by_itw( ppath_t,  atmosphere_dim, p_grid, lat_grid, 
                              lon_grid, t_field, "t_field", ppath.gp_p, 
                              ppath.gp_lat, ppath.gp_lon, itw_field );
      //
      // - VMR fields:
      const Index ns = vmr_field.nbooks();
      ppath_vmr.resize(ns,np);
      for( Index is=0; is<ns; is++ )
        {
          interp_atmfield_by_itw( ppath_vmr(is, joker), atmosphere_dim,
            p_grid, lat_grid, lon_grid, vmr_field( is, joker, joker,  joker ), 
            "vmr_field", ppath.gp_p, ppath.gp_lat, ppath.gp_lon, itw_field );
        }
      //-----------------------------------------------------------------------

      // Execute the *rte_agenda*
      rte_agenda.execute( agenda_verb );
    }
}



//! rte_step_std
/*!
    Solves monochromatic VRTE for an atmospheric slab with constant 
    conditions.

    The function can be used for clearsky and cloudbox calculations.

    The function is best explained by considering a homogenous layer. That is,
    the physical conditions inside the layer are constant. In reality they
    are not constant, so in practical all coefficients have to be averaged 
    before calling this function. 
    Total extinction and absorption inside the layer are described by
    *ext_mat_av* and *abs_vec_av* respectively,
    the blackbdody radiation of the layer is given by *rte_planck_value*
    and the propagation path length through the layer is *l_step*.

    There is an additional scattering source term in the 
    VRTE, the scattering integral term. For this function a constant
    scattering term is assumed. The radiative transfer step is only a part 
    the iterative solution of the scattering problem, for more 
    information consider AUG. In the clearsky case this variable has to be
    set to 0.

    When calling the function, the vector *stokes_vec* shall contain the
    Stokes vector for the incoming radiation. The function returns this
    vector, then containing the outgoing radiation on the other side of the 
    layer.

    The function performs the calculations differently depending on the
    conditions to improve the speed. There are three cases: <br>
       1. Scalar absorption (stokes_dim = 1). <br>
       2. The matrix ext_mat_gas is diagonal (unpolarised absorption). <br>
       3. The total general case.

    \param   stokes_vec         Input/Output: A Stokes vector.
    \param   trans_mat          Input/Output: Transmission matrix of slab.
    \param   ext_mat_av         Input: Averaged extinction matrix.
    \param   abs_vec_av         Input: Averaged absorption vector.
    \param   sca_vec_av         Input: averaged scattering vector.
    \param   l_step             Input: The length of the RTE step.
    \param   rte_planck_value   Input: Blackbody radiation.

    \author Claudia Emde and Patrick Eriksson, 
    \date   2002-11-22
*/
void rte_step_std(//Output and Input:
              VectorView stokes_vec,
              MatrixView trans_mat,
              //Input
              ConstMatrixView ext_mat_av,
              ConstVectorView abs_vec_av,
              ConstVectorView sca_vec_av,
              const Numeric& l_step,
              const Numeric& rte_planck_value )
{
  //Stokes dimension:
  Index stokes_dim = stokes_vec.nelem();

  //Check inputs:
  assert(is_size(trans_mat, stokes_dim, stokes_dim));
  assert(is_size(ext_mat_av, stokes_dim, stokes_dim));
  assert(is_size(abs_vec_av, stokes_dim));
  assert(is_size(sca_vec_av, stokes_dim));
  assert( rte_planck_value >= 0 );
  assert( l_step >= 0 );
  assert (!is_singular( ext_mat_av ));


  // Check, if only the first component of abs_vec is non-zero:
  bool unpol_abs_vec = true;

  for (Index i = 1; i < stokes_dim; i++)
    if (abs_vec_av[i] != 0)
      unpol_abs_vec = false;

  bool unpol_sca_vec = true;

  for (Index i = 1; i < stokes_dim; i++)
    if (sca_vec_av[i] != 0)
      unpol_sca_vec = false;


  //--- Scalar case: ---------------------------------------------------------
  if( stokes_dim == 1 )
    {
      trans_mat(0,0) = exp(-ext_mat_av(0,0) * l_step);
      stokes_vec[0]  = stokes_vec[0] * trans_mat(0,0) +
                       (abs_vec_av[0] * rte_planck_value + sca_vec_av[0]) /
        ext_mat_av(0,0) * (1 - trans_mat(0,0));
    }


  //--- Vector case: ---------------------------------------------------------

  // We have here two cases, diagonal or non-diagonal ext_mat_gas
  // For diagonal ext_mat_gas, we expect abs_vec_gas to only have a
  // non-zero value in position 1.

  //- Unpolarised
  else if( is_diagonal(ext_mat_av) && unpol_abs_vec && unpol_sca_vec )
    {
      trans_mat      = 0;
      trans_mat(0,0) = exp(-ext_mat_av(0,0) * l_step);

      // Stokes dim 1
      //   assert( ext_mat_av(0,0) == abs_vec_av[0] );
      //   Numeric transm = exp( -l_step * abs_vec_av[0] );
      stokes_vec[0] = stokes_vec[0] * trans_mat(0,0) +
                      (abs_vec_av[0] * rte_planck_value + sca_vec_av[0]) /
                      ext_mat_av(0,0) * (1 - trans_mat(0,0));

      // Stokes dims > 1
      for( Index i=1; i<stokes_dim; i++ )
        {
          //      assert( abs_vec_av[i] == 0.);
          trans_mat(i,i) = trans_mat(0,0);
          stokes_vec[i]  = stokes_vec[i] * trans_mat(i,i) +
                       sca_vec_av[i] / ext_mat_av(i,i)  * (1 - trans_mat(i,i));
        }
    }


  //- General case
  else
    {
      //Initialize internal variables:

      // Matrix LU used for LU decompostion and as dummy variable:
      Matrix LU(stokes_dim, stokes_dim);
      ArrayOfIndex indx(stokes_dim); // index for pivoting information
      Vector b(stokes_dim); // dummy variable
      Vector x(stokes_dim); // solution vector for K^(-1)*b
      Matrix I(stokes_dim, stokes_dim);

      Vector B_abs_vec(stokes_dim);
      B_abs_vec = abs_vec_av;
      B_abs_vec *= rte_planck_value;

      for (Index i=0; i<stokes_dim; i++)
        b[i] = B_abs_vec[i] + sca_vec_av[i];  // b = abs_vec * B + sca_vec

      // solve K^(-1)*b = x
      ludcmp(LU, indx, ext_mat_av);
      lubacksub(x, LU, b, indx);

      Matrix ext_mat_ds(stokes_dim, stokes_dim);
      ext_mat_ds = ext_mat_av;
      ext_mat_ds *= -l_step; // ext_mat_ds = -ext_mat*ds

      Index q = 10;  // index for the precision of the matrix exp function
      //Matrix exp_ext_mat(stokes_dim, stokes_dim);
      //matrix_exp(exp_ext_mat, ext_mat_ds, q);
      matrix_exp( trans_mat, ext_mat_ds, q);

      Vector term1(stokes_dim);
      Vector term2(stokes_dim);

      id_mat(I);
      for(Index i=0; i<stokes_dim; i++)
        {
          for(Index j=0; j<stokes_dim; j++)
            LU(i,j) = I(i,j) - trans_mat(i,j); // take LU as dummy variable
          // LU(i,j) = I(i,j) - exp_ext_mat(i,j); // take LU as dummy variable
        }
      mult(term2, LU, x); // term2: second term of the solution of the RTE with
                          //fixed scattered field

      // term1: first term of solution of the RTE with fixed scattered field
      //mult(term1, exp_ext_mat, stokes_vec);
      mult( term1, trans_mat, stokes_vec );

      for (Index i=0; i<stokes_dim; i++)
        stokes_vec[i] = term1[i] + term2[i];  // Compute the new Stokes Vector
    }
}



void rte_step_std_clearsky(
              //Output and Input:
                    VectorView   stokes_vec,
                    MatrixView   trans_mat,
              //Input
              const bool&        abs_polarised,
              ConstMatrixView    ext_mat,
              ConstVectorView    abs_vec,
              const Numeric&     l_step,
              const Numeric&     b )
{
  //Stokes dimension:
  Index stokes_dim = stokes_vec.nelem();

  //Check inputs:
  assert( is_size( trans_mat, stokes_dim, stokes_dim ) ); 
  assert( is_size( ext_mat, stokes_dim, stokes_dim ) ); 
  assert( is_size( abs_vec, stokes_dim ) );
  assert( b >= 0 );
  assert( l_step >= 0 );


  //--- Unpolarised absorption -----------------------------------------------
  if( !abs_polarised )
    {
      // Init transmission matrix to zero
      trans_mat      = 0;

      // Stokes dim 1
      trans_mat(0,0) = exp( -ext_mat(0,0) * l_step );
      stokes_vec[0]  = stokes_vec[0] * trans_mat(0,0) + 
                                                    b * ( 1 - trans_mat(0,0) );

      // Higher Stokes dims
      for( Index i=1; i<stokes_dim; i++ )
        {
          trans_mat(i,i) = trans_mat(0,0); 
          stokes_vec[i]  *= trans_mat(i,i);
        }
      
    }

  //--- Polarised absorption -------------------------------------------------
  else
    { 
      // We do not allow scalar case here
      assert( stokes_dim > 1 );

      throw runtime_error( "Polarised absorption not yet handled.");
    }
}



//! rte_std
/*! 
   Core function for the different versions of WSM RteStd. 

   See the the online help (arts -d RteStd)

   All input and output arguments are identical to the WSV with identical name,
   beside:

    \param   do_transmission   Input: Boolean to fill *ppath_transmissions* or 
                               not.

   \author Patrick Eriksson
   \date   2005-05-19
*/
void rte_std(
             Matrix&         iy,
             Vector&         emission,
             Matrix&         abs_vec,
             Tensor3&        ext_mat,
             Matrix&         abs_scalar_gas,
             Numeric&        rte_pressure,
             Numeric&        rte_temperature,
             Vector&         rte_vmr_list,
             Index&          f_index,
             Index&          ppath_index,
             Tensor4&        ppath_transmissions,
             Tensor4&        diy_dvmr,
             Tensor3&        diy_dt,
       const Ppath&          ppath,
       const Vector&         ppath_p,
       const Vector&         ppath_t,
       const Matrix&         ppath_vmr,
       const Vector&         f_grid,
       const Index&          stokes_dim,
       const Agenda&         emission_agenda,
       const Agenda&         scalar_gas_absorption_agenda,
       const Agenda&         opt_prop_gas_agenda,
       const ArrayOfIndex&   rte_do_vmr_jacs,
       const Index&          rte_do_t_jacs,
       const bool&           do_transmissions )
{
  // Temporary variables
  bool   abs_polarised = false;   // To become WSV?

  // Relevant checks are assumed to be done in RteCalc

  // Some sizes
  const Index   nf = f_grid.nelem();
  const Index   np = ppath.np;
  const Index   ns = ppath_vmr.nrows();    // Number of species

  // Init output variables
  rte_vmr_list.resize(ns);

  // Log of pressure
  Vector   logp_ppath(np);
  transform( logp_ppath, log, ppath_p  );

  // If f_index < 0, scalar gas absorption is calculated for 
  // all frequencies in f_grid.
  f_index = -1;
      
  // Jacobian variables
  //
  bool   do_vmr_jacobians = false; 
  bool   do_t_jacobians   = false;
  const Index   ng = rte_do_vmr_jacs.nelem();
  //
  if( ng )
    {
      do_vmr_jacobians = true;
      diy_dvmr.resize(nf,stokes_dim,np,ng);
      diy_dvmr = 0.0;
    }
  //
  if( rte_do_t_jacs )
    {
      throw runtime_error("Analytical temperature jacobians not yet handled.");
      do_t_jacobians = true;
      diy_dt.resize(nf,stokes_dim,np);
      diy_dt = 0.0;
    }

  // Transmission variables
  Matrix    trans(stokes_dim,stokes_dim);
  bool      save_transmissions = false;
  bool      any_abs_polarised = false;
  //
  if( do_transmissions  ||  do_vmr_jacobians  || do_t_jacobians ) 
    {
      save_transmissions = true;
      ppath_transmissions.resize(np-1,nf,stokes_dim,stokes_dim); 
    }


  // Loop the propagation path steps
  //
  // The number of path steps is np-1.
  // The path points are stored in such way that index 0 corresponds to
  // the point closest to the sensor.
  for( Index ip=np-1; ip>0; ip-- )
    {
      // Calculate mean of atmospheric parameters
      rte_pressure    = exp( 0.5 * ( logp_ppath[ip] + logp_ppath[ip-1] ) );
      rte_temperature = 0.5*(ppath_t[ip] + ppath_t[ip-1]);
      for( Index is=0; is<ns; is++ )
        { rte_vmr_list[is] = 0.5*(ppath_vmr(is,ip)+ppath_vmr(is, ip-1)); }
      
      // The absO2ZeemanModel needs the position in the propagation
      // path. 
      ppath_index = ip;

      // Call agendas for RT properties
      emission_agenda.execute( ip );
      scalar_gas_absorption_agenda.execute( ip );
      opt_prop_gas_agenda.execute( ip ); 

      // Polarised absorption?
      if( abs_polarised )
        { any_abs_polarised = true; }

      // Loop frequencies
      for( Index iv=0; iv<nf; iv++ )
        {
          // Jacobians
          if( do_vmr_jacobians )
            {
              const Numeric   dd = 
                       -0.5 * ppath.l_step[ip-1] * ( iy(iv,0) - emission[iv] );
              for( Index ig=0; ig<ng; ig++ )
                {
                  const Index   is = rte_do_vmr_jacs[ig];
                  const Numeric p  = dd*abs_scalar_gas(iv,is)/rte_vmr_list[is];
                  diy_dvmr(iv,0,ip,is)   += p;
                  diy_dvmr(iv,0,ip-1,is) += p;
                }
            }

          // Perform the RTE step.
          rte_step_std_clearsky( iy(iv,joker), trans, abs_polarised, 
                                 ext_mat(iv,joker,joker), abs_vec(iv,joker), 
                                 ppath.l_step[ip-1], emission[iv] );

          if( save_transmissions )
            { ppath_transmissions(ip-1,iv,joker,joker) = trans; }
        }
    }

  // Postprocessing of Jacobians
  if( do_vmr_jacobians )
    {
      for( Index iv=0; iv<nf; iv++ )
        {
          // Transmission of 1
          id_mat( trans );

          if( any_abs_polarised )
            {
              Matrix  mtmp( stokes_dim, stokes_dim );
              Vector  vtmp( stokes_dim );

              for( Index ip=0; ip<np-1; ip++ )
                {
                  mtmp = trans;
                  mult( trans, ppath_transmissions(ip,iv,joker,joker), mtmp );
                  for( Index ig=0; ig<ng; ig++ )
                    {
                      const Index is = rte_do_vmr_jacs[ig];
                      vtmp = diy_dvmr(iv,joker,ip+1,is); 
                      mult( diy_dvmr(iv,joker,ip+1,is), trans, vtmp );
                    }
                }
            }
          else
            {
              for( Index ip=0; ip<np-1; ip++ )
                {
                  for( Index ii=0; ii<stokes_dim; ii++ )
                    {
                      trans(ii,ii) *= ppath_transmissions(ip,iv,ii,ii);
                      for( Index ig=0; ig<rte_do_vmr_jacs.nelem(); ig++ )
                        {
                          diy_dvmr(iv,ii,ip+1,rte_do_vmr_jacs[ig]) *= 
                                                                  trans(ii,ii);
                        }
                    }
                }
            }
        }
    }
}



//! surface_calc
/*!
    Weights together downwelling radiation and surface emission.

    *iy* must have correct size when function is called.

    \param   iy                 In/Out: Radiation matrix, amtching 
                                        the WSV with the same name.
    \param   I                  Input: Downwelling radiation, with dimensions
                                       matching: 
                                       (surface_los, f_grid, sokes_dim)
    \param   surface_los        Input: As the WSV with the same name.
    \param   surface_rmatrix    Input: As the WSV with the same name.
    \param   surface_emission   Input: As the WSV with the same name.

    \author Patrick Eriksson 
    \date   2005-04-07
*/
void surface_calc(
              Matrix&         iy,
        const Tensor3&        I,
        const Matrix&         surface_los,
        const Tensor4&        surface_rmatrix,
        const Matrix&         surface_emission )
{
  // Some sizes
  const Index   nf         = I.nrows();
  const Index   stokes_dim = I.ncols();
  const Index   nlos       = surface_los.nrows();

  iy = surface_emission;
  
  /*
  cout << surface_emission << "\n";
  cout << iy << "\n";
  cout << I << "\n";
  */

  // Loop *surface_los*-es. If no such LOS, we are ready.
  if( nlos > 0 )
    {
      for( Index ilos=0; ilos<nlos; ilos++ )
        {
          Vector rtmp(stokes_dim);  // Reflected Stokes vector for 1 frequency

          for( Index iv=0; iv<nf; iv++ )
            {
          mult( rtmp, surface_rmatrix(ilos,iv,joker,joker), I(ilos,iv,joker) );
          iy(iv,joker) += rtmp;
            }
        }
    }
}



//! surface_specular_los
/*!
    Calculates the LOS for a specular surface reflection.

    \param   los                In/Out: LOS to be modified.
    \param   atmosphere_dim     Input: As the WSV with the same name.

    \author Patrick Eriksson 
    \date   2002-09-22
*/
void surface_specular_los(
              VectorView   los,
        const Index&       atmosphere_dim )
{
  assert( atmosphere_dim >= 1  &&  atmosphere_dim <= 3 );

  if( atmosphere_dim == 1 )
    {
      assert( los.nelem() == 1 );
      assert( los[0] > 90 );      // Otherwise surface refl. not possible
      assert( los[0] <= 180 ); 

      los[0] = 180 - los[0];
    }

  else if( atmosphere_dim == 2 )
    {
      assert( los.nelem() == 1 );
      assert( abs(los[0]) <= 180 ); 

      los[0] = sign( los[0] ) * 180 - los[0];
    }

  else if( atmosphere_dim == 3 )
    {
      assert( los.nelem() == 2 );
      assert( los[0] >= 0 ); 
      assert( los[0] <= 180 ); 
      assert( abs( los[1] ) <= 180 ); 

      // Calculate LOS neglecting any tilt of the surface
      los[0] = 180 - los[0];
      los[1] = los[1];
    }
}



//! surface_specular_R_and_b
/*!
    Sets up the surface reflection matrix and emission vector for
    the case of specular reflection.

    The function handles only one frequency at the time.

    See further the surface chapter in the user guide.

    \param   surface_rmatrix   Out: As the WSV with the same name, but slice
                                    for one direction and one frequency.
    \param   surface_emission  Out: As the WSV with the same name, but slice
                                    for one direction and one frequency.
    \param   Rv                In: Complex amplitude relection coefficient
                                   for vertical polarisation.
    \param   Rh                In: Complex amplitude relection coefficient
                                   for horisontal polarisation.
    \paran   f                 In: Frequency (a scalar).
    \param   stokes_dim        In: As the WSV with the same name.
    \param   surface_skin_t    In: As the WSV with the same name.

    \author Patrick Eriksson 
    \date   2004-09-24
*/
void surface_specular_R_and_b(
              MatrixView   surface_rmatrix,
              VectorView   surface_emission,
        const Complex&     Rv,
        const Complex&     Rh,
        const Numeric&     f,
        const Index&       stokes_dim,
        const Numeric&     surface_skin_t )
{
  assert( surface_rmatrix.nrows() == stokes_dim );
  assert( surface_rmatrix.ncols() == stokes_dim );
  assert( surface_emission.nelem() == stokes_dim );

  // Expressions are derived in the surface chapter in the user guide

  const Numeric   rv    = pow( abs(Rv), 2.0 );
  const Numeric   rh    = pow( abs(Rh), 2.0 );
  const Numeric   rmean = ( rv + rh ) / 2;
  const Numeric   B     = planck( f, surface_skin_t );

  surface_rmatrix   = 0.0;
  surface_emission  = 0.0;

  surface_rmatrix(0,0) = rmean;
  surface_emission[0]  = B * ( 1 - rmean );

  if( stokes_dim > 1 )
    {
      const Numeric   rdiff = ( rv - rh ) / 2;

      surface_rmatrix(1,0) = rdiff;
      surface_rmatrix(0,1) = rdiff;
      surface_rmatrix(1,1) = rmean;
      surface_emission[1]  = -B * rdiff;

        if( stokes_dim > 2 )
          {
            const Complex   a     = Rh * conj(Rv);
            const Complex   b     = Rv * conj(Rh);
            const Numeric   c     = real( a + b ) / 2.0;

            surface_rmatrix(2,2) = c;
      
            if( stokes_dim > 3 )
              {
                const Numeric   d     = imag( a - b ) / 2.0;

                surface_rmatrix(3,2) = d;
                surface_rmatrix(2,3) = d;
                surface_rmatrix(3,3) = c;
              }
          }
    }
  /*
  cout << Rv << "\n";
  cout << Rh << "\n";
  cout << surface_rmatrix << "\n";
  cout << surface_emission << "\n";
  */
}



