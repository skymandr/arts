/*!
  \file   m_optproperties.cc
  \author Sreerekha T.R. <rekha@uni-bremen.de>, 
          Claudia Emde <claudia@sat.physik.uni-bremen.de>
          Cory Davies <cory@met.ed.ac.uk>
  \date   Mon Jun 10 11:19:11 2002 
  \brief  This filecontains workspace methods for calculating the optical 
  properties for the radiative transfer. 

  Optical properties are the extinction matrix, absorption vector and
  scattering vector.  The optical properties for the gases can be
  calculated with or without Zeeman effect.
*/

/*===========================================================================
  === External declarations
  ===========================================================================*/

#include <cmath>
#include "arts.h"
#include "exceptions.h"
#include "array.h"
#include "matpackIII.h"
#include "matpackVII.h"
#include "logic.h"
#include "interpolation.h"
#include "messages.h"
#include "xml_io.h"
#include "optproperties.h"
#include "math_funcs.h"
#include "sorting.h"

extern const Numeric PI;
extern const Numeric DEG2RAD;
extern const Numeric RAD2DEG;

#define part_type scat_data_raw[i_pt].ptype
#define f_datagrid scat_data_raw[i_pt].f_grid
#define T_datagrid scat_data_raw[i_pt].T_grid
#define za_datagrid scat_data_raw[i_pt].za_grid
#define aa_datagrid scat_data_raw[i_pt].aa_grid
#define pha_mat_data_raw scat_data_raw[i_pt].pha_mat_data  //CPD: changed from pha_mat_data
#define ext_mat_data_raw scat_data_raw[i_pt].ext_mat_data  //which wouldn't let me play with
#define abs_vec_data_raw scat_data_raw[i_pt].abs_vec_data  //scat_data_mono.


//! Calculates phase matrix for the single particle types.
/*! 
  In this function the phase matrix calculated
  in the laboratory frame. This function is used in the calculation
  of the scattering integral (*scat_fieldCalc*).
  
  The interpolation of the data on the actual frequency is the first 
  step in this function. 
  
  Then the transformation from the database coordinate system to to 
  laboratory coordinate system is done.

  WS Output:
  \param pha_mat_spt Extinction matrix for a single particle type.
  WS Input:
  \param scat_data_raw Raw data containing optical properties.
  \param scat_za_grid Zenith angle grid for scattering calculation.
  \param scat_aa_grid Azimuth angle grid for scattering calculation.
  \param scat_za_index Index specifying the propagation direction.
  \param scat_aa_index  Index specifying the azimuth angle
  \param f_index Index specifying the frequency.
  \param f_grid Frequency grid.

  \author Claudia Emde
  \date   2003-05-14
*/

void pha_mat_sptFromData( // Output:
                         Tensor5& pha_mat_spt,
                         // Input:
                         const ArrayOfSingleScatteringData& scat_data_raw,
                         const Vector& scat_za_grid,
                         const Vector& scat_aa_grid,
                         const Index& scat_za_index, // propagation directions
                         const Index& scat_aa_index,
                         const Index& f_index,
                         const Vector& f_grid,
                         const Numeric& t_value
                         )
{
    out3 << "Calculate *pha_mat_spt* from database\n";

  const Index N_pt = scat_data_raw.nelem();
  const Index stokes_dim = pha_mat_spt.ncols();

  if (stokes_dim > 4 || stokes_dim < 1){
    throw runtime_error("The dimension of the stokes vector \n"
                         "must be 1,2,3 or 4");
  }
  
  assert( pha_mat_spt.nshelves() == N_pt );
  
  // Phase matrix in laboratory coordinate system. Dimensions:
  // [frequency, za_inc, aa_inc, stokes_dim, stokes_dim]
  Tensor5 pha_mat_data_int;
  

  // Loop over the included particle_types
  for (Index i_pt = 0; i_pt < N_pt; i_pt++)
    {
      // First we have to transform the data from the coordinate system 
      // used in the database (depending on the kind of particle type 
      // specified by *ptype*) to the laboratory coordinate sytem. 
      
      // Frequency interpolation:
     
      // The data is interpolated on one frequency. 
      pha_mat_data_int.resize(pha_mat_data_raw.nshelves(), pha_mat_data_raw.nbooks(),
                              pha_mat_data_raw.npages(), pha_mat_data_raw.nrows(), 
                              pha_mat_data_raw.ncols());

      
      // Gridpositions:
      GridPos freq_gp;
      gridpos(freq_gp, f_datagrid, f_grid[f_index]);

      GridPos t_gp;
      gridpos(t_gp, T_datagrid, t_value);

      // Interpolationweights:
      Vector itw(4);
      interpweights(itw, freq_gp, t_gp);
     
      for (Index i_za_sca = 0; i_za_sca < pha_mat_data_raw.nshelves() ; i_za_sca++)
        {
          for (Index i_aa_sca = 0; i_aa_sca < pha_mat_data_raw.nbooks(); i_aa_sca++)
            {
              for (Index i_za_inc = 0; i_za_inc < pha_mat_data_raw.npages(); 
                   i_za_inc++)
                {
                  for (Index i_aa_inc = 0; i_aa_inc < pha_mat_data_raw.nrows(); 
                       i_aa_inc++)
                    {  
                      for (Index i = 0; i < pha_mat_data_raw.ncols(); i++)
                        {
                          pha_mat_data_int(i_za_sca, 
                                                i_aa_sca, i_za_inc, 
                                                i_aa_inc, i) =
                            interp(itw,
                                   pha_mat_data_raw(joker, joker, i_za_sca, 
                                                i_aa_sca, i_za_inc, 
                                                i_aa_inc, i),
                                   freq_gp, t_gp);
                        }
                    }
                }
            }
        }
      
// Do the transformation into the laboratory coordinate system.
       for (Index za_inc_idx = 0; za_inc_idx < scat_za_grid.nelem(); 
            za_inc_idx ++)
         {
           for (Index aa_inc_idx = 0; aa_inc_idx < scat_aa_grid.nelem(); 
                aa_inc_idx ++) 
             {
               pha_matTransform(pha_mat_spt
                                (i_pt, za_inc_idx, aa_inc_idx, joker, joker),
                                pha_mat_data_int, 
                                za_datagrid, aa_datagrid,
                                part_type, scat_za_index, scat_aa_index,
                                za_inc_idx, 
                                aa_inc_idx, scat_za_grid, scat_aa_grid);
             }
         }
    }
}
  

//! Calculates phase matrix for the single particle types.
/*! 
  In this function the phase matrix calculated
  in the laboratory frame. This function can be used in the agenda
  *pha_mat_spt_agenda* which is part of the calculation of the scattering
  integral (*scat_fieldCalc*).
 
  The function uses *pha_mat_sptDOITOpt*, which is the phase matrix interpolated
  on the right frequency for all scattering angles.
  
  The transformation from the database coordinate system to to 
  laboratory coordinate system is done in this function.

    WS Output:
  \param pha_mat_spt Phase matrix for a single particle type.
  WS Input:
  \param pha_mat_sptDOITOpt Phase matrix data.
  \param scat_theta Scattering angles.
  \param scat_za_grid Zenith angle grid for scattering calculation.
  \param scat_aa_grid Azimuth angle grid for scattering calculation.
  \param scat_za_index Index specifying the propagation direction.
  \param scat_aa_index  Index specifying the azimuth angle
  
  \author Claudia Emde
  \date   2003-08-25
*/

void pha_mat_sptFromDataDOITOpt( // Output:
                         Tensor5& pha_mat_spt,
                         // Input:
                         const ArrayOfTensor6& pha_mat_sptDOITOpt,
                         const ArrayOfSingleScatteringData& scat_data_raw,
                         const Tensor4& scat_theta,
                         const Index& za_grid_size,
                         const Vector& scat_aa_grid,
                         const Index& scat_za_index, // propagation directions
                         const Index& scat_aa_index,
                         const Numeric& t_value
                         )
{

  // Create equidistant zenith angle grid
  Vector za_grid;
  nlinspace(za_grid, 0, 180, za_grid_size);  
  
  const Numeric za_sca = za_grid[scat_za_index];
  const Numeric aa_sca = scat_aa_grid[scat_aa_index];
  const Index N_pt = pha_mat_sptDOITOpt.nelem();

  Numeric za_inc;
  Numeric aa_inc;
  
  // Do the transformation into the laboratory coordinate system.
  for (Index i_pt = 0; i_pt < N_pt; i_pt ++)
    { 
      // Temperature interpolation
      // Gridpositions:
      GridPos T_gp;
      gridpos(T_gp, T_datagrid, t_value); 
      
      // Interpolationweights:
      Vector itw(2);
      interpweights(itw, T_gp);

      Vector pha_mat_int(6);
            
      for (Index za_inc_idx = 0; za_inc_idx < za_grid_size; za_inc_idx ++)
        {
          for (Index aa_inc_idx = 0; aa_inc_idx < scat_aa_grid.nelem();
               aa_inc_idx ++) 
            {
              // Loop over matrix elements, for p20 always 8
              for (Index i = 0; i<6; i++)
                {
                  pha_mat_int[i] = 
                    interp( itw, 
                            pha_mat_sptDOITOpt[i_pt](joker, scat_za_index,
                                               scat_aa_index, za_inc_idx, 
                                               aa_inc_idx, i),
                            T_gp);
                }
              
              const Numeric theta_rad = scat_theta
                (scat_za_index, scat_aa_index, za_inc_idx, aa_inc_idx);
              
              za_inc = za_grid[za_inc_idx];
              aa_inc = scat_aa_grid[aa_inc_idx];
              
              pha_mat_labCalc(pha_mat_spt(i_pt, za_inc_idx, aa_inc_idx, 
                                          joker, joker),
                              pha_mat_int, za_sca, aa_sca, za_inc, aa_inc, 
                              theta_rad);
            }
        }
    }

}


    
//! Calculates opticle properties for the single particle types.
/*! 
  In this function extinction matrix and absorption vector are calculated
  in the laboratory frame. These properties are required for the RT
  calculation, inside the the i_fieldUpdateXXX functions. 
  
  The interpolation of the data on the actual frequency is the first 
  step in this function. 
  
  Then the transformation from the database coordinate system to to 
  laboratory coordinate system is done.

  Output of the function are *ext_mat_spt*, and *abs_vec_spt* which hold 
  the optical properties for a specified propagation direction for 
  each particle type. 

  WS Output:
  \param ext_mat_spt Extinction matrix for a single particle type.
  \param abs_vec_spt Absorption vector for a single particle type.
  WS Input:
  \param scat_data_raw Raw data containing optical properties.
  \param scat_za_grid Zenith angle grid for scattering calculation.
  \param scat_aa_grid Azimuth angle grid for scattering calculation.
  \param scat_za_index Index specifying the propagation direction.
  \param scat_aa_index  Index specifying the azimuth angle
  \param f_index Index specifying the frequency.
  \param f_grid Frequency grid.

  \author Claudia Emde
  \date   2003-05-14
*/
void opt_prop_sptFromData( // Output and Input:
                         Tensor3& ext_mat_spt,
                         Matrix& abs_vec_spt,
                         // Input:
                         const ArrayOfSingleScatteringData& scat_data_raw,
                         const Vector& scat_za_grid,
                         const Vector& scat_aa_grid,
                         const Index& scat_za_index, // propagation directions
                         const Index& scat_aa_index,
                         const Index& f_index,
                         const Vector& f_grid,
                         const Numeric& t_value
                         )
{
  
  const Index N_pt = scat_data_raw.nelem();
  const Index stokes_dim = ext_mat_spt.ncols();
  const Numeric za_sca = scat_za_grid[scat_za_index];
  const Numeric aa_sca = scat_aa_grid[scat_aa_index];

  if (stokes_dim > 4 || stokes_dim < 1){
    throw runtime_error("The dimension of the stokes vector \n"
                         "must be 1,2,3 or 4");
  }
  
  assert( ext_mat_spt.npages() == N_pt );
  assert( abs_vec_spt.nrows() == N_pt );

  // Phase matrix in laboratory coordinate system. Dimensions:
  // [frequency, za_inc, aa_inc, stokes_dim, stokes_dim]
  Tensor3 ext_mat_data_int;
  Tensor3 abs_vec_data_int;
  
   // Initialisation
  ext_mat_spt = 0.;
  abs_vec_spt = 0.;


  // Loop over the included particle_types
  for (Index i_pt = 0; i_pt < N_pt; i_pt++)
    {
      // First we have to transform the data from the coordinate system 
      // used in the database (depending on the kind of particle type 
      // specified by *ptype*) to the laboratory coordinate sytem. 
      
      // Frequency interpolation:
     
      // The data is interpolated on one frequency. 
      //
      // Resize the variables for the interpolated data:
      //
      ext_mat_data_int.resize(ext_mat_data_raw.npages(),
                              ext_mat_data_raw.nrows(), 
                              ext_mat_data_raw.ncols());
      //
      abs_vec_data_int.resize(abs_vec_data_raw.npages(),
                              abs_vec_data_raw.nrows(), 
                              abs_vec_data_raw.ncols());
      
      
      // Gridpositions:
      GridPos freq_gp;
      gridpos(freq_gp, f_datagrid, f_grid[f_index]); 

      GridPos t_gp;
      gridpos(t_gp, T_datagrid, t_value);

      // Interpolationweights:
      Vector itw(4);
      interpweights(itw, freq_gp, t_gp);
     
      for (Index i_za_sca = 0; i_za_sca < ext_mat_data_raw.npages() ; i_za_sca++)
        {
          for(Index i_aa_sca = 0; i_aa_sca < ext_mat_data_raw.nrows(); i_aa_sca++)
            {
              //
              // Interpolation of extinction matrix:
              //
              for (Index i = 0; i < ext_mat_data_raw.ncols(); i++)
                {
                  ext_mat_data_int(i_za_sca, i_aa_sca, i) =
                    interp(itw, ext_mat_data_raw(joker, joker, i_za_sca, i_aa_sca, i),
                           freq_gp, t_gp);
                }
            }
        }

      for (Index i_za_sca = 0; i_za_sca < abs_vec_data_raw.npages() ; i_za_sca++)
        {
          for(Index i_aa_sca = 0; i_aa_sca < abs_vec_data_raw.nrows(); i_aa_sca++)
            {
              //
              // Interpolation of absorption vector:
              //
              for (Index i = 0; i < abs_vec_data_raw.ncols(); i++)
                {
                  abs_vec_data_int(i_za_sca, i_aa_sca, i) =
                    interp(itw, abs_vec_data_raw(joker, joker, i_za_sca, i_aa_sca, i),
                           freq_gp, t_gp);
                }
            }
        }
      

      //
      // Do the transformation into the laboratory coordinate system.
      //
      // Extinction matrix:
      //
     
  
      ext_matTransform(ext_mat_spt(i_pt, joker, joker),
                       ext_mat_data_int,
                       za_datagrid, aa_datagrid, part_type,
                       za_sca, aa_sca);
      // 
      // Absorption vector:
      //
      abs_vecTransform(abs_vec_spt(i_pt, joker),
                       abs_vec_data_int,
                       za_datagrid, aa_datagrid, part_type,
                       za_sca, aa_sca);                
    }


}
                          

//! Extinction Coefficient Matrix for the particle 
/*! 
  This function sums up the extinction matrices for all particle 
  types weighted with particle number density
  \param ext_mat Output and input : physical extinction coefficient 
  for the particles for given angles. 
  \param ext_mat_spt Input : extinction matrix for the single particle type
  \param pnd_field Input : particle number density givs the local 
  concentration for all particles.
  \param atmosphere_dim Input : he atmospheric dimensionality (now 1 or 3)
  \param scat_p_index Input : Pressure index for scattering calculations.
  \param scat_lat_index Input : Latitude index for scattering calculations.
  \param scat_lon_index Input : Longitude index for scattering calculations.

  \author Sreerekha Ravi
*/
void ext_matAddPart(
                      Tensor3& ext_mat,
                      const Tensor3& ext_mat_spt,
                      const Tensor4& pnd_field,
                      const Index& atmosphere_dim,
                      const Index& scat_p_index,
                      const Index& scat_lat_index,
                      const Index& scat_lon_index) 
                     
{
  Index N_pt = ext_mat_spt.npages();
  Index stokes_dim = ext_mat_spt.nrows();
  
  Matrix ext_mat_part(stokes_dim, stokes_dim, 0.0);

  
  if (stokes_dim > 4 || stokes_dim < 1){
    throw runtime_error(
                        "The dimension of stokes vector can be "
                        "only 1,2,3, or 4");
  }
  if ( ext_mat_spt.ncols() != stokes_dim){
    
    throw runtime_error(" The columns of ext_mat_spt should "
                        "agree to stokes_dim");
  }

  if (atmosphere_dim == 1)
    {
      // this is a loop over the different particle types
      for (Index l = 0; l < N_pt; l++)
        { 
          
          // now the last two loops over the stokes dimension.
          for (Index m = 0; m < stokes_dim; m++)
            {
              for (Index n = 0; n < stokes_dim; n++)
               //summation of the product of pnd_field and 
                //ext_mat_spt.
              ext_mat_part(m, n) += 
                (ext_mat_spt(l, m, n) * pnd_field(l, scat_p_index, 0, 0));
            }
        }

      //Add particle extinction matrix to *ext_mat*.
      ext_mat(0, Range(joker), Range(joker)) += ext_mat_part;
    }
 
  if (atmosphere_dim == 3)
    {
      
      // this is a loop over the different particle types
      for (Index l = 0; l < N_pt; l++)
        { 
          
          // now the last two loops over the stokes dimension.
          for (Index m = 0; m < stokes_dim; m++)
            {
              for (Index n = 0; n < stokes_dim; n++)
                 //summation of the product of pnd_field and 
                //ext_mat_spt.
                ext_mat_part(m, n) +=  (ext_mat_spt(l, m, n) * 
                                        pnd_field(l, scat_p_index, 
                                                  scat_lat_index, 
                                                  scat_lon_index));
              
            } 
        }

      //Add particle extinction matrix to *ext_mat*.
      ext_mat(0, Range(joker), Range(joker)) += ext_mat_part;

    }

} 

//! Absorption Vector for the particle 
/*! 
  This function sums up the absorption vectors for all particle 
  types weighted with particle number density
  \param abs_vec Output : physical absorption vector 
  for the particles for given angles. 
  \param abs_vec_spt Input : absorption for the single particle type
  \param pnd_field Input : particle number density givs the local 
  concentration for all particles.
  \param atmosphere_dim Input : the atmospheric dimensionality (now 1 or 3)
  \param scat_p_index Input : Pressure index for scattering calculations.
  \param scat_lat_index Input : Latitude index for scattering calculations.
  \param scat_lon_index Input : Longitude index for scattering calculations.

  \author Sreerekha Ravi
*/
void abs_vecAddPart(
                      Matrix& abs_vec,
                      const Matrix& abs_vec_spt,
                      const Tensor4& pnd_field,
                      const Index& atmosphere_dim,
                      const Index& scat_p_index,
                      const Index& scat_lat_index,
                      const Index& scat_lon_index) 
                    
{
  Index N_pt = abs_vec_spt.nrows();
  Index stokes_dim = abs_vec_spt.ncols();

  Vector abs_vec_part(stokes_dim, 0.0);

  if ((stokes_dim > 4) || (stokes_dim <1)){
    throw runtime_error("The dimension of stokes vector "
                        "can be only 1,2,3, or 4");
  } 
 
  if (atmosphere_dim == 1)
    {
      // this is a loop over the different particle types
      for (Index l = 0; l < N_pt ; ++l)
        {
          // now the loop over the stokes dimension.
          //(CE:) in the middle was l instead of m
          for (Index m = 0; m < stokes_dim; ++m)
             //summation of the product of pnd_field and 
            //abs_vec_spt.
            abs_vec_part[m] += 
              (abs_vec_spt(l, m) * pnd_field(l, scat_p_index, 0, 0));
          
        }
      //Add the particle absorption
      abs_vec(0, Range(joker)) += abs_vec_part;
    }
  
  if (atmosphere_dim == 3)
    {
      // this is a loop over the different particle types
      for (Index l = 0; l < N_pt ; ++l)
        {
          
          // now the loop over the stokes dimension.
          for (Index m = 0; m < stokes_dim; ++m)
             //summation of the product of pnd_field and 
            //abs_vec_spt.
            abs_vec_part[m] += (abs_vec_spt(l, m) *
                                pnd_field(l, scat_p_index,
                                          scat_lat_index, 
                                          scat_lon_index));
          
        }
      //Add the particle absorption
      abs_vec(0,Range(joker)) += abs_vec_part;
    }
} 

//! Initialize extinction matrix.
/*!
  This method is necessary, because all other extinction methods just
  add to the existing extinction matrix. 

  So, here we have to make it the right size and fill it with 0.
  
  Note, that the matrix is not really a matrix, because it has a
  leading frequency dimension.

  \param ext_mat Extinction matrix.
  \param f_grid Frequency grid.
  \param stokes_dim Stokes dimension.
  \param f_index Frequency index.

  \author Stefan Buehler

  \date   2002-12-12
*/
void ext_matInit( Tensor3&      ext_mat,
                  const Vector& f_grid,
                  const Index&  stokes_dim,
                  const Index&   f_index)
{
  Index freq_dim;

  if( f_index < 0 )
    freq_dim = f_grid.nelem();
  else
    freq_dim = 1;
 
  ext_mat.resize( freq_dim,
                  stokes_dim,
                  stokes_dim );
  ext_mat = 0;                  // Initialize to zero!

  out2 << "Set dimensions of ext_mat as ["
       << freq_dim << ","
       << stokes_dim << ","
       << stokes_dim << "] and initialized to 0.\n";
}

//! Add gas absorption to all diagonal elements of extinction matrix.
/*! 
  The task of this method is to sum up the gas absorption of the
  different gas species and add the result to the extinction matrix. 

  \param ext_mat Input and Output: Extinction matrix.
                 Dimension: [#frequencies, stokes_dim, stokes_dim]
  
  \param abs_scalar_gas Scalar gas absorption coefficients.
                        Dimension: [#frequencies, #gas_species]

  \author Stefan Buehler
          (rewritten version of function by Sreerekha Ravi)

  \date   2002-12-12
*/
void ext_matAddGas( Tensor3&      ext_mat,
                    const Matrix& abs_scalar_gas )
{
  // Number of Stokes parameters:
  const Index stokes_dim = ext_mat.ncols();

  // The second dimension of ext_mat must also match the number of
  // Stokes parameters:
  if ( stokes_dim != ext_mat.nrows() )
    throw runtime_error("Row dimension of ext_mat inconsistent with "
                        "column dimension."); 

  // Number of frequencies:
  const Index f_dim = ext_mat.npages();

  // This must be consistent with the first dimension of
  // abs_scalar_gas. Check this:
  if ( f_dim != abs_scalar_gas.nrows() )
    throw runtime_error("Frequency dimension of ext_mat and abs_scalar_gas\n"
                        "are inconsistent.");

  // Sum up absorption over all species.
  // This gives us an absorption vector for all frequencies. Of course
  // this includes the special case that there is only one frequency.
  Vector abs_total(f_dim);
  for ( Index i=0; i<f_dim; ++i )
    abs_total[i] = abs_scalar_gas(i,joker).sum();

  for ( Index i=0; i<stokes_dim; ++i )
    {
      // Add the absorption value to all the diagonal elements:
      ext_mat(joker,i,i) += abs_total[joker];
      
      // We don't have to do anything about the off-diagonal
      // elements! 
    }
}

//! Initialize absorption vector.
/*!
  This method is necessary, because all other absorption methods just
  add to the existing absorption vector.

  So, here we have to make it the right size and fill it with 0.

  Note, that the vector is not really a vector, because it has a
  leading frequency dimension.
  
  \param abs_vec Extinction matrix.
  \param f_grid Frequency grid.
  \param stokes_dim Stokes dimension.
  \param f_index Frequency index.

  \author Stefan Buehler

  \date   2002-12-12
*/
void abs_vecInit( Matrix&       abs_vec,
                  const Vector& f_grid,
                  const Index&  stokes_dim,
                  const Index&  f_index)
{
  Index freq_dim;

  if( f_index < 0 )
    freq_dim = f_grid.nelem();
  else
    freq_dim = 1;
 
  abs_vec.resize( freq_dim,
                  stokes_dim );
  abs_vec = 0;                  // Initialize to zero!

  out2 << "Set dimensions of abs_vec as ["
       << freq_dim << ","
       << stokes_dim << "] and initialized to 0.\n";
}

//! Add gas absorption to first element of absorption vector.
/*! 
  The task of this method is to sum up the gas absorption of the
  different gas species and add the result to the first element of the
  absorption vector.

  \param abs_vec Input and Output: Absorption vector
                 Dimension: [#frequencies, stokes_dim]
  
  \param abs_scalar_gas Scalar gas absorption coefficients.
                        Dimension: [#frequencies, #gas_species]

  \author Stefan Buehler
          (rewritten version of function by Sreerekha Ravi)

  \date   2002-12-12
*/
void abs_vecAddGas( Matrix&       abs_vec,
                    const Matrix& abs_scalar_gas )
{
  // Number of frequencies:
  const Index f_dim = abs_vec.nrows();

  // This must be consistent with the first dimension of
  // abs_scalar_gas. Check this:
  if ( f_dim != abs_scalar_gas.nrows() )
    throw runtime_error("Frequency dimension of abs_vec and abs_scalar_gas\n"
                        "are inconsistent.");

  // Loop all frequencies. Of course this includes the special case
  // that there is only one frequency.
  for ( Index i=0; i<f_dim; ++i )
    {
      // Sum up the columns of abs_scalar_gas and add to the first
      // element of abs_vec.
      abs_vec(i,0) += abs_scalar_gas(i,joker).sum();
    }

  // We don't have to do anything about higher elements of abs_vec,
  // since scalar gas absorption only influences the first element.
}

//! Add Zeeman extinction  to the elements of extinction matrix.
/*! 
  
   \param ext_mat Input and Output: Extinction matrix.
   Dimension: [#frequencies, stokes_dim, stokes_dim]
   
   \param ext_mat_zee Input : Zeeman extinction coefficient matrix.
   Dimension: [#frequencies, stokes_dim, stokes_dim]
   
   \author Sreerekha Ravi
   (based on ext_matAddGas by Stefan Buehler)
   
   \date   2003-12-01
*/
void ext_matAddGasZeeman( Tensor3&      ext_mat,
			  const Tensor3&  ext_mat_zee )
{
  // Number of Stokes parameters:
  const Index stokes_dim = ext_mat.ncols();

  // The second dimension of ext_mat must also match the number of
  // Stokes parameters:
  if ( stokes_dim != ext_mat.nrows() )
    throw runtime_error("Row dimension of ext_mat inconsistent with "
                        "column dimension."); 

  for ( Index i=0; i<stokes_dim; ++i )
    {
      for ( Index j=0; j<stokes_dim; ++j )
	{
	  // Add the zeeman extinction to extinction matrix.
	  ext_mat(joker,i,j) += ext_mat_zee(joker, i, j);
	}
      
    }
}

//! Add zeeman absorption to the elements of absorption vector.
/*! 
  The task of this method is to sum up the gas absorption of the
  different gas species and add the result to the first element of the
  absorption vector.

  \param abs_vec Input and Output: Absorption vector
                 Dimension: [#frequencies, stokes_dim]
  
  \param abs_vec_zee Input : Zeeman absorption vector.
                        Dimension: [#frequencies, #gas_species]

  \author Sreerekha Ravi
          (based on abs_vecAddGas written by Stefan Buehler)

  \date   2003-12-01
*/
void abs_vecAddGasZeeman( Matrix&      abs_vec,
			  const Matrix& abs_vec_zee )
{
  // Number of Stokes parameters:
  const Index stokes_dim = abs_vec_zee.ncols();
  // that there is only one frequency.
  for ( Index j=0; j<stokes_dim; ++j )
    {
      abs_vec(joker,j) += abs_vec_zee(joker,j);
    }
}


//! Phase Matrix for the particle 
/*! 
  
  This function sums up the phase matrices for all particle 
  types weighted with particle number density
  \param pha_mat Output : physical phase matrix 
  for the particles for given angles. 
  \param pha_mat_spt Input : phase matrix for the single particle type
  \param pnd_field Input : particle number density givs the local 
  concentration for all particles.
  \param atmosphere_dim Input : he atmospheric dimensionality (now 1 or 3)
  \param scat_p_index Input : Pressure index for scattering calculations.
  \param scat_lat_index Input : Latitude index for scattering calculations.
  \param scat_lon_index Input : Longitude index for scattering calculations.

  \author Sreerekha Ravi
*/
void pha_matCalc(
                 Tensor4& pha_mat,
                 const Tensor5& pha_mat_spt,
                 const Tensor4& pnd_field,
                 const Index& atmosphere_dim,
                 const Index& scat_p_index,
                 const Index& scat_lat_index,
                 const Index& scat_lon_index) 
                      
{

  Index N_pt = pha_mat_spt.nshelves();
  Index Nza = pha_mat_spt.nbooks();
  Index Naa = pha_mat_spt.npages();
  Index stokes_dim = pha_mat_spt.nrows();
 
  pha_mat.resize(Nza, Naa, stokes_dim, stokes_dim);

  // Initialisation
  for (Index za_index = 0; za_index < Nza; ++ za_index)
    {
      for (Index aa_index = 0; aa_index < Naa; ++ aa_index)
        {
          for (Index stokes_index_1 = 0; stokes_index_1 < stokes_dim;
               ++ stokes_index_1)
            {
              for (Index stokes_index_2 = 0; stokes_index_2 < stokes_dim; 
                   ++ stokes_index_2)
                pha_mat(za_index, aa_index, stokes_index_1, stokes_index_2)

                  = 0.0;
            }
        }
    }
  if (atmosphere_dim == 1)
    {
      // this is a loop over the different particle types
      for (Index pt_index = 0; pt_index < N_pt; ++ pt_index)
        {
                  
          // these are loops over zenith angle and azimuth angle
          for (Index za_index = 0; za_index < Nza; ++ za_index)
            {
              for (Index aa_index = 0; aa_index < Naa; ++ aa_index)
                {
                  
                  // now the last two loops over the stokes dimension.
                  for (Index stokes_index_1 = 0; stokes_index_1 < stokes_dim; 
                       ++  stokes_index_1)
                    {
                      for (Index stokes_index_2 = 0; stokes_index_2 < stokes_dim;
                           ++ stokes_index_2)
                         //summation of the product of pnd_field and 
                          //pha_mat_spt.
                        pha_mat(za_index, aa_index,  
                                     stokes_index_1, stokes_index_2) += 
                          
                          (pha_mat_spt(pt_index, za_index, aa_index,  
                                       stokes_index_1, stokes_index_2) * 
                           pnd_field(pt_index,scat_p_index, 0, 0));
                    }
                }
            }
        }
    }
          
  if (atmosphere_dim == 3)
    {
      // this is a loop over the different particle types
      for (Index pt_index = 0; pt_index < N_pt; ++ pt_index)
        {
          
          // these are loops over zenith angle and azimuth angle
          for (Index za_index = 0; za_index < Nza; ++ za_index)
            {
              for (Index aa_index = 0; aa_index < Naa; ++ aa_index)
                {
                  
                  // now the last two loops over the stokes dimension.
                  for (Index i = 0;  i < stokes_dim; ++  i)
                    {
                      for (Index j = 0; j < stokes_dim; ++ j)
                        {
                          //summation of the product of pnd_field and 
                          //pha_mat_spt.
                          pha_mat(za_index, aa_index, i,j ) += 
                            (pha_mat_spt(pt_index, za_index, aa_index, i, j) * 
                             pnd_field(pt_index, scat_p_index,  
                                       scat_lat_index, scat_lon_index));
                          
                          
                        } 
                    }	
                }
            }		
        }	
    }		
}


//! Check the scattering data in the database.
/*! 
  This function can be used to check datafiles containing data for 
  randomly oriented scattering media.
  It is checked whether the data is consistent. The integral over the phase 
  matrix should result the scattering cross section \<C_sca\>.
  
  The check is if:
  \<C_ext\> - \<C_sca\> = \<C_abs\>
  
  The result is printed on the screen.
  
  \param scat_data_raw (Input) Single scattering data.
  
  \author Claudia Emde
  \date   2003-05-20
*/
void scat_data_rawCheck(//Input:
                         const ArrayOfSingleScatteringData& scat_data_raw
                         )
{

  xml_write_to_file("SingleScatteringData", scat_data_raw);
  
  const Index N_pt = scat_data_raw.nelem();
  
  // Loop over the included particle_types
  for (Index i_pt = 0; i_pt < N_pt; i_pt++)
    {
      Numeric Csca = AngIntegrate_trapezoid
        (pha_mat_data_raw(0, 0, joker, 0, 0, 0, 0), za_datagrid);

      Numeric Cext = ext_mat_data_raw(0,0,0,0,0);

      Numeric Cabs = Cext - Csca;

      Numeric Cabs_data = abs_vec_data_raw(0,0,0,0,0);

      Numeric Csca_data = Cext - Cabs_data;

      out1 << " Coefficients in database: \n"
           << "Cext: " << Cext << " Cabs: " << Cabs_data << " Csca: " << Csca_data  
           << " \n Calculated absorption cooefficient: \n"
           << "Cabs calculated: " << Cabs   
           << " Csca: " << Csca << "\n";
      
    }
}


//! Prepare single scattering data for a DOIT scattering calculation.
/*! 
  This function has can be used for scatttering calcualtions using the 
  DOIT method. It is a method optimized for randomly oriented 
  scattering media and it can only be used for such cases.
  It has to be used in combination with *pha_mat_sptFromDataDOITOpt*.

  It has to be used in *scat_mono_agenda*. The phase matrix data is 
  interpolated on the actual frequency and on all scattering angles 
  following from all possible combinations in *scat_za_grid* and 
  *scat_aa_grid*. The temperature interpolation in done 
  *pha_mat_sptFromDataDOITOpt*.
  
  Output:
  \param scat_theta All scattering angles.
  \param pha_mat_sptDOITOpt Interpolated phase matrix data.
    Input:
  \param scat_za_grid Zenith angle grid for scattering calculations.
  \param scat_aa_grid Azimuth angle grid for scattering calculations.
  \param scat_data_raw Array of single scattering data for all hydrometeor
                      species.
  \param f_grid Frequency grid.
  \param f_index Frequency index. 
  \param atmosphere_dim Atmospheric dimension. 

  \author Claudia Emde
  \date   2003-08-25
*/
void ScatteringDataPrepareDOITOpt( //Output:
                                   Tensor4& scat_theta,
                                   ArrayOfTensor6& pha_mat_sptDOITOpt,
                                   //Input:
                                   const Index& za_grid_size,
                                   const Vector& scat_aa_grid,
                                   const ArrayOfSingleScatteringData& scat_data_raw,
                                   const Vector& f_grid,
                                   const Index& f_index,
                                   const Index& atmosphere_dim
                                   )
{

  // Check, whether single scattering data contains the right frequencies:
  for (Index i = 0; i<scat_data_raw.nelem(); i++)
    {
      if (scat_data_raw[i].f_grid[0] > f_grid[f_index] || 
          scat_data_raw[i].f_grid[scat_data_raw[i].f_grid.nelem()-1] 
          < f_grid[f_index])
        {
          ostringstream os;
          os << "Frequency of the scattering calculation " << f_grid[f_index] 
             << " GHz is not contained \n in the frequency grid of the " << i+1 
             << "th single scattering data file \n(*ParticleTypeAdd*). "
             << "Range:"  << scat_data_raw[i].f_grid[0]/1e9 <<" - "
             << scat_data_raw[i].f_grid[scat_data_raw[i].f_grid.nelem()-1]/1e9
             <<" GHz \n";
          throw runtime_error( os.str() );
        }
    }

  // For 1D calculation the scat_aa dimension is not required:
  Index N_aa_sca;
  if(atmosphere_dim == 1)
    N_aa_sca = 1;
  else
    N_aa_sca = scat_aa_grid.nelem();
  
  Vector za_grid;
  nlinspace(za_grid, 0, 180, za_grid_size);

  Index N_pt = scat_data_raw.nelem();  
  
  // Initialize variables:
  scat_theta.resize(za_grid.nelem(), N_aa_sca, 
                    za_grid.nelem(),scat_aa_grid.nelem());
  
  for (Index i_pt = 0; i_pt < N_pt; i_pt++)
    {
      Index N_T = T_datagrid.nelem();
      pha_mat_sptDOITOpt[i_pt].resize(N_T,  za_grid_size, N_aa_sca, 
                    za_grid_size, scat_aa_grid.nelem(), 6);
      
      // Frequency interpolation of the data:
      Tensor3 pha_mat_data_int(N_T, pha_mat_data_raw.nshelves(), 6);
      
      // Gridpositions frequency:
      GridPos freq_gp;
      gridpos(freq_gp, f_datagrid, f_grid[f_index]); 
      
      // Interpolationweights temperature:
      Vector freq_itw(2);
      interpweights(freq_itw, freq_gp);
      
      for (Index i = 0; i < 6; i++)
        {
          for (Index t_idx = 0; t_idx < N_T; t_idx ++)
            {
              for (Index za_idx = 0; za_idx < za_datagrid.nelem(); za_idx ++)
                {
                  pha_mat_data_int(t_idx, za_idx, i) =
                    interp(freq_itw, pha_mat_data_raw(joker, t_idx,
                                                  za_idx, 0, 0, 0, i),
                       freq_gp);
                }
            }
        }
      
      // Calculate all scattering angles for all combinations of incoming 
      // and scattered directions and interpolation.
      
      Numeric za_sca_rad, za_inc_rad, aa_sca_rad, aa_inc_rad;
      Numeric theta_rad;
      Numeric theta;
      GridPos thet_gp;
      Vector itw(2);
  
      for (Index t_idx = 0; t_idx < N_T; t_idx ++)
        {
          for (Index za_sca = 0; za_sca < za_grid_size; za_sca ++)
            {
              for (Index aa_sca = 0; aa_sca < N_aa_sca; aa_sca ++)
                {
                  for (Index za_inc = 0; za_inc < za_grid_size; za_inc ++)
                    {
                      for (Index aa_inc = 0; aa_inc < scat_aa_grid.nelem(); aa_inc ++)
                        {
                          za_sca_rad = za_grid[za_sca] * DEG2RAD;
                          za_inc_rad = za_grid[za_inc] * DEG2RAD;
                          aa_sca_rad = scat_aa_grid[aa_sca] * DEG2RAD;
                          aa_inc_rad = scat_aa_grid[aa_inc] * DEG2RAD;
                          
                          theta_rad = acos(cos(za_sca_rad) * cos(za_inc_rad) + 
                                           sin(za_sca_rad) * sin(za_inc_rad) * 
                                           cos(aa_sca_rad - aa_inc_rad));
                          
                          scat_theta(za_sca, aa_sca, za_inc, aa_inc)
                            = theta_rad;
                          
                          theta = theta_rad * RAD2DEG;
                      
                          gridpos(thet_gp, za_datagrid, theta);
                          interpweights(itw, thet_gp);
                          
                          for (Index i = 0; i < 6; i++)
                            {
                              pha_mat_sptDOITOpt[i_pt](t_idx,  za_sca, 
                                                 aa_sca, za_inc, aa_inc, i)
                                = interp(itw, pha_mat_data_int(t_idx, joker,
                                                               i), 
                                     thet_gp);
                            }
                        }
                    }
                }
            }
        }
    } 
}

 

//! scat_data_monoCalc
/*! 
  calculates monochromatic single scattering data for all particle types by
  interpolating scat_data_raw over frequency.

  \param scat_data_mono     Output: monochomatic scattering data
  \param scat_data_raw      raw scattering data WSV
  \param f_grid
  \param f_index

  \author Cory Davis
  \date 2003-12-16
*/
void scat_data_monoCalc(
			ArrayOfSingleScatteringData& scat_data_mono,
			const ArrayOfSingleScatteringData& scat_data_raw,
			const Vector& f_grid,
			const Index& f_index
                        )
{
  // Check, whether single scattering data contains the right frequencies:
  for (Index i = 0; i<scat_data_raw.nelem(); i++)
    {
      if (scat_data_raw[i].f_grid[0] > f_grid[f_index] || 
          scat_data_raw[i].f_grid[scat_data_raw[i].f_grid.nelem()-1] 
          < f_grid[f_index])
        {
          ostringstream os;
          os << "Frequency of the scattering calculation " << f_grid[f_index] 
             << " GHz is not contained \nin the frequency grid of the " << i+1 
             << "the single scattering data file \n(*ParticleTypeAdd*). "
             << "Range:"  << scat_data_raw[i].f_grid[0]/1e9 <<" - "
             << scat_data_raw[i].f_grid[scat_data_raw[i].f_grid.nelem()-1]/1e9
             <<" GHz \n";
          throw runtime_error( os.str() );
        }
    }

  const Index N_pt = scat_data_raw.nelem();
  
  //Initialise scat_data_mono
  scat_data_mono.resize(N_pt);
  
  // Loop over the included particle_types
  for (Index i_pt = 0; i_pt < N_pt; i_pt++)
    {
      for (Index t_index = 0; t_index < T_datagrid.nelem(); t_index ++)
        {
          
          // Gridpositions:
          GridPos freq_gp;
          gridpos(freq_gp, f_datagrid, f_grid[f_index]); 
      
          // Interpolationweights:
          Vector itw(2);
          interpweights(itw, freq_gp);

          //Stuff that doesn't need interpolating
          scat_data_mono[i_pt].ptype=part_type;
          scat_data_mono[i_pt].f_grid.resize(1);
          scat_data_mono[i_pt].f_grid=f_grid[f_index];
          scat_data_mono[i_pt].T_grid=scat_data_raw[i_pt].T_grid;
          scat_data_mono[i_pt].za_grid=za_datagrid;
          scat_data_mono[i_pt].aa_grid=aa_datagrid;
          
          //Phase matrix data
          scat_data_mono[i_pt].pha_mat_data.resize(1, T_datagrid.nelem(),
                                                   pha_mat_data_raw.nshelves(),
                                                   pha_mat_data_raw.nbooks(),
                                                   pha_mat_data_raw.npages(),
                                                   pha_mat_data_raw.nrows(),
                                                   pha_mat_data_raw.ncols());
          
          for (Index i_za_sca = 0; i_za_sca < pha_mat_data_raw.nshelves();
               i_za_sca++)
            {
              for (Index i_aa_sca = 0; i_aa_sca < pha_mat_data_raw.nbooks();
                   i_aa_sca++)
                {
                  for (Index i_za_inc = 0; i_za_inc < 
                         pha_mat_data_raw.npages(); 
                       i_za_inc++)
                    {
                      for (Index i_aa_inc = 0; 
                           i_aa_inc < pha_mat_data_raw.nrows(); 
                           i_aa_inc++)
                        {  
                          for (Index i = 0; i < pha_mat_data_raw.ncols(); i++)
                            {
                              scat_data_mono[i_pt].pha_mat_data(0, t_index, 
                                                                i_za_sca, 
                                                                i_aa_sca,
                                                                i_za_inc, 
                                                                i_aa_inc, i) =
                                interp(itw,
                                       pha_mat_data_raw(joker, t_index,
                                                        i_za_sca, 
                                                        i_aa_sca, i_za_inc, 
                                                        i_aa_inc, i),
                                       freq_gp);
                            }
                        }
                    }
                }
            }
          //Extinction matrix data
          scat_data_mono[i_pt].ext_mat_data.resize(1, T_datagrid.nelem(), 
                                                   ext_mat_data_raw.npages(),
                                                   ext_mat_data_raw.nrows(),
                                                   ext_mat_data_raw.ncols());
          for (Index i_za_sca = 0; i_za_sca < ext_mat_data_raw.npages();
               i_za_sca++)
            {
              for(Index i_aa_sca = 0; i_aa_sca < ext_mat_data_raw.nrows();
                  i_aa_sca++)
                {
                  //
                  // Interpolation of extinction matrix:
                  //
                  for (Index i = 0; i < ext_mat_data_raw.ncols(); i++)
                    {
                      scat_data_mono[i_pt].ext_mat_data(0, t_index, 
                                                        i_za_sca, i_aa_sca, i)
                        = interp(itw, ext_mat_data_raw(joker, t_index, i_za_sca,
                                                       i_aa_sca, i),
                                 freq_gp);
                    }
                }
            }
          //Absorption vector data
          scat_data_mono[i_pt].abs_vec_data.resize(1, T_datagrid.nelem(),
                                                   abs_vec_data_raw.npages(),
                                                   abs_vec_data_raw.nrows(),
                                                   abs_vec_data_raw.ncols());
          for (Index i_za_sca = 0; i_za_sca < abs_vec_data_raw.npages() ;
               i_za_sca++)
            {
              for(Index i_aa_sca = 0; i_aa_sca < abs_vec_data_raw.nrows();
                  i_aa_sca++)
                {
                  //
                  // Interpolation of absorption vector:
                  //
                  for (Index i = 0; i < abs_vec_data_raw.ncols(); i++)
                    {
                      scat_data_mono[i_pt].abs_vec_data(0, t_index, i_za_sca,
                                                        i_aa_sca, i) =
                        interp(itw, abs_vec_data_raw(joker, t_index, i_za_sca,
                                                     i_aa_sca, i),
                               freq_gp);
                    }
                }
            }
        }
    }
}



//! Calculates opticle properties for the single particle types.
/*! 
  This is essentially a monochromatic version of opt_prop_sptFromData. 
ext_mat_spt and abs_vec_spt are calculated from scat_data_mono instead of 
scat_data_raw

\param ext_mat_spt  Output: extinction matrix for each particle at the given
                         propagation direction
\param abs_vec_spt  Output: absorption coefficient vector at the given
                         propagation direction
\param scat_data_mono monochromatic single scattering data
\param scat_za_grid Workspace variable
\param scat_aa_grid Workspace variable
\param scat_za_index Workspace variable
\param scat_aa_index Workspace variable

\author Cory Davis
\date 2003-12-17

*/
void opt_prop_sptFromMonoData( // Output and Input:
                         Tensor3& ext_mat_spt,
                         Matrix& abs_vec_spt,
                         // Input:
                         const ArrayOfSingleScatteringData& scat_data_mono,
                         const Vector& scat_za_grid,
                         const Vector& scat_aa_grid,
                         const Index& scat_za_index, // propagation directions
                         const Index& scat_aa_index
                         )
{
  const Index N_pt = scat_data_mono.nelem();
  const Index stokes_dim = ext_mat_spt.ncols();
  const Numeric za_sca = scat_za_grid[scat_za_index];
  const Numeric aa_sca = scat_aa_grid[scat_aa_index];

  if (stokes_dim > 4 || stokes_dim < 1){
    throw runtime_error("The dimension of the stokes vector \n"
                         "must be 1,2,3 or 4");
  }
  
  assert( ext_mat_spt.npages() == N_pt );
  assert( abs_vec_spt.nrows() == N_pt );

  // Initialisation
  ext_mat_spt = 0.;
  abs_vec_spt = 0.;


  // Loop over the included particle_types
  for (Index i_pt = 0; i_pt < N_pt; i_pt++)
    {
      // First we have to transform the data from the coordinate system 
      // used in the database (depending on the kind of particle type 
      // specified by *ptype*) to the laboratory coordinate sytem. 
      
      //
      // Do the transformation into the laboratory coordinate system.
      //
      // Extinction matrix:
      //
     
  
      ext_matTransform(ext_mat_spt(i_pt, joker, joker),
                       scat_data_mono[i_pt].ext_mat_data(0,0,joker,joker,joker),
                       scat_data_mono[i_pt].za_grid, 
		       scat_data_mono[i_pt].aa_grid, 
		       scat_data_mono[i_pt].ptype,
                       za_sca, aa_sca);
      // 
      // Absorption vector:
      //
      abs_vecTransform(abs_vec_spt(i_pt, joker),
                       scat_data_mono[i_pt].abs_vec_data(0,0, joker,joker,joker),
                       scat_data_mono[i_pt].za_grid, 
		       scat_data_mono[i_pt].aa_grid, 
		       scat_data_mono[i_pt].ptype,
                       za_sca, aa_sca);                
    }


}
 

//! Calculates phase matrix for the single particle types.
/*! 
  In this function the phase matrix calculated
  in the laboratory frame. This function is used in the calculation
  of the scattering integral (*scat_fieldCalc*).
  
  This  function is a monochromatic version of pha_mat_sptFromData.
  
  WS Output:
  \param pha_mat_spt Extinction matrix for a single particle type.
  WS Input:
  \param scat_data_mono Raw data containing optical properties.
  \param scat_za_grid Zenith angle grid for scattering calculation.
  \param scat_aa_grid Azimuth angle grid for scattering calculation.
  \param scat_za_index Index specifying the propagation direction.
  \param scat_aa_index  Index specifying the azimuth angle
  \param f_index Index specifying the frequency.
  \param f_grid Frequency grid.

  \author Claudia Emde
  \date   2004-02-11
*/                        
void pha_mat_sptFromMonoData( // Output:
                         Tensor5& pha_mat_spt,
                         // Input:
                         const ArrayOfSingleScatteringData& scat_data_mono,
                         const Index& za_grid_size,
                         const Vector& scat_aa_grid,
                         const Index& scat_za_index, // propagation directions
                         const Index& scat_aa_index,
                         const Numeric& t_value
                         )
{
  out3 << "Calculate *pha_mat_spt* from scat_data_mono. \n";
  
  Vector za_grid;
  nlinspace(za_grid, 0, 180, za_grid_size); 

  const Index N_pt = scat_data_mono.nelem();
  const Index stokes_dim = pha_mat_spt.ncols();

  if (stokes_dim > 4 || stokes_dim < 1){
    throw runtime_error("The dimension of the stokes vector \n"
                         "must be 1,2,3 or 4");
  }
  
  assert( pha_mat_spt.nshelves() == N_pt );
  
  Tensor5 scat_data_int;

  // Temperature interpolation
   for (Index i_pt = 0; i_pt < N_pt; i_pt ++)
     { 
       
      // Temperature interpolation
      // Gridpositions:
      GridPos T_gp;
      gridpos(T_gp, scat_data_mono[i_pt].T_grid, t_value); 
      
      // Interpolationweights:
      Vector itw(2);
      interpweights(itw, T_gp);
      
      for (Index za = 0; za < scat_data_mono[i_pt].pha_mat_data.nshelves();
           za++)
        {
          for (Index aa = 0; aa < scat_data_mono[i_pt].pha_mat_data.nbooks(); 
               aa++)
            {
              for (Index za_i = 0; za_i < 
                     scat_data_mono[i_pt].pha_mat_data.npages();
                   za_i++)
                {
                  for (Index aa_i = 0; aa_i <
                         scat_data_mono[i_pt].pha_mat_data.nrows(); aa_i++)
                    {
                      for (Index i = 0; i < 
                             scat_data_mono[i_pt].pha_mat_data.ncols(); i++)
                        {
                          scat_data_int(za, aa, za_i, aa_i, i) =
                            interp( itw,
                                    scat_data_mono[i_pt].pha_mat_data
                                    (0, joker, za, aa, za_i, aa_i, i), 
                                    T_gp);
                        }
                    }
                }
            }
        }
         
      // Initialisation
      pha_mat_spt = 0.;
      
      // Do the transformation into the laboratory coordinate system.
      for (Index za_inc_idx = 0; za_inc_idx < za_grid_size;
           za_inc_idx ++)
        {
          for (Index aa_inc_idx = 0; aa_inc_idx < scat_aa_grid.nelem();
               aa_inc_idx ++) 
            {
              pha_matTransform( pha_mat_spt
                                (i_pt, za_inc_idx, aa_inc_idx, joker, joker),
                                scat_data_int,
                                scat_data_mono[i_pt].za_grid, 
                                scat_data_mono[i_pt].aa_grid,
                                scat_data_mono[i_pt].ptype,
                                scat_za_index, scat_aa_index, 
                                za_inc_idx, 
                                aa_inc_idx, za_grid, scat_aa_grid);
            }
        }
     }
}
  
