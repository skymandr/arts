/* Copyright (C) 2002 Stefan Buehler  <sbuehler@uni-bremen.de>

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

/*!
  \file   logic.cc
  \author Stefan Buehler <sbuehler@uni-bremen.de>
  \date   Fri May  3 19:02:36 2002
  
  \brief  Logical functions.
  
  All functions here have return type bool. They all check whether some
  condition is fullfilled and return true if that is the case.

  These functions are intended to be used either inside "if" statements
  or inside "assert" statements.

  The condition should have a simple and intuitive meaning!
*/



#include <math.h> 
#include <stdexcept>
#include "logic.h"


// For checking, if a Numeric equal zero we have to take into account the
// numerical precicion. If a value is smaller than *precision* it is 
// taken to be 0.
#define precision 0.


//! Checks if a variable equals 0 or 1.
/*!
  \return       True if the variable is 0 or 1. Otherwise false.
  \param    x   A variable of type Index.
*/
bool is_bool( const Index& x )
{
  return ( x==0 || x==1 );
}



//! Checks if an integer is a multiple of another integer.
/*!
  The function returns true if y * n = x, where n is an integer.

  The choice of y = 0 is not allowed.

   \return       True if x is a multiple of y.
   \param    x   Nominator of the integer division.
   \param    y   Denominator of the integer division.

   \author Patrick Eriksson 
   \date   2002-08-11 
*/
bool is_multiple( const Index& x, const Index& y )
{
  assert( y != 0 );
  return ( 0 == fmod( Numeric(x), Numeric(y) ) );
}



//! Verifies that the size of x is l.
/*! 
  This function is supposed to be used together with assert like this:
  assert(is_size(x,l)) 

  \param  x The Vector to check.
  \param  n The desired length.
  \return True if the size of x is l.
*/
bool is_size( ConstVectorView   x,
              const Index&      n ) 
{
  return( n == x.nelem() );
}

//! Verifies that the size of x is r by c.
/*! 
  \param  x The Matrix to check.
  \param  r The desired number of rows.
  \param  c The desired number of columns.
  \return True if the size of x is r x c.
*/
bool is_size( ConstMatrixView   x,
              const Index&      r,
              const Index&      c ) 
{
  return( r == x.nrows() &&
          c == x.ncols()     );
}

//! Verifies that the size of x is [p,r,c].
/*! 
  \param  x The Tensor to check.
  \param  p The desired number of pages.
  \param  r The desired number of rows.
  \param  c The desired number of columns.
  \return True if the size of x is correct.
*/
bool is_size( ConstTensor3View  x,
              const Index&      p,
              const Index&      r,
              const Index&      c ) 
{
  return( p == x.npages()     &&
          r == x.nrows()      &&
          c == x.ncols()     );
}

//! Verifies that the size of x is [b,p,r,c].
/*! 
  \param  x The Tensor to check.
  \param  b The desired number of books.
  \param  p The desired number of pages.
  \param  r The desired number of rows.
  \param  c The desired number of columns.
  \return True if the size of x is correct.
*/
bool is_size( ConstTensor4View  x,
              const Index&      b,
              const Index&      p,
              const Index&      r,
              const Index&      c ) 
{
  return( b == x.nbooks()     &&
          p == x.npages()     &&
          r == x.nrows()      &&
          c == x.ncols()     );
}

//! Verifies that the size of x is [s,b,p,r,c].
/*! 
  \param  x The Tensor to check.
  \param  s The desired number of shelves.
  \param  b The desired number of books.
  \param  p The desired number of pages.
  \param  r The desired number of rows.
  \param  c The desired number of columns.
  \return True if the size of x is correct.
*/
bool is_size( ConstTensor5View  x,
              const Index&      s,
              const Index&      b,
              const Index&      p,
              const Index&      r,
              const Index&      c ) 
{
  return( s == x.nshelves()   &&
          b == x.nbooks()     &&
          p == x.npages()     &&
          r == x.nrows()      &&
          c == x.ncols()     );
}

//! Verifies that the size of x is [v,s,b,p,r,c].
/*! 
  \param  x The Tensor to check.
  \param  v The desired number of vitrines.
  \param  s The desired number of shelves.
  \param  b The desired number of books.
  \param  p The desired number of pages.
  \param  r The desired number of rows.
  \param  c The desired number of columns.
  \return True if the size of x is correct.
*/
bool is_size( ConstTensor6View  x,
              const Index&      v,
              const Index&      s,
              const Index&      b,
              const Index&      p,
              const Index&      r,
              const Index&      c ) 
{
  return( v == x.nvitrines()  &&
          s == x.nshelves()   &&
          b == x.nbooks()     &&
          p == x.npages()     &&
          r == x.nrows()      &&
          c == x.ncols()     );
}

//! Verifies that the size of x is [l,v,s,b,p,r,c].
/*! 
  \param  x The Tensor to check.
  \param  l The desired number of libraries.
  \param  v The desired number of vitrines.
  \param  s The desired number of shelves.
  \param  b The desired number of books.
  \param  p The desired number of pages.
  \param  r The desired number of rows.
  \param  c The desired number of columns.
  \return True if the size of x is correct.
*/
bool is_size( ConstTensor7View  x,
              const Index&      l,
              const Index&      v,
              const Index&      s,
              const Index&      b,
              const Index&      p,
              const Index&      r,
              const Index&      c ) 
{
  return( l == x.nlibraries() &&
          v == x.nvitrines()  &&
          s == x.nshelves()   &&
          b == x.nbooks()     &&
          p == x.npages()     &&
          r == x.nrows()      &&
          c == x.ncols()     );
}

//! Checks if a vector is sorted in ascending order.
/*!
  Duplicated values are allowed.

  \param   x   A vector.
  \return      True if sorted.
*/
bool is_sorted( ConstVectorView   x )
{
  if( x.nelem() > 1 )
    {
      for( Index i=1; i<x.nelem(); i++ )
        {
          if( x[i] < x[i-1] )
            return false;
        }
    }
  return true;
}

//! Checks if a vector is sorted and strictly increasing. 
/*! 
    Duplicated values are not allowed.

    \return      True if strictly increasing, otherwise false.
    \param   x   A vector.
*/
bool is_increasing( ConstVectorView   x )
{
  if( x.nelem() > 1 )
    {
      for( Index i=1; i<x.nelem(); i++ )
        {
          if( x[i] <= x[i-1] )
            return false;
        }
    }
  return true;
}

//! Checks if a vector is sorted in reversed order and is strictly decreasing.
/*! 
    Duplicated values are not allowed.

    \return      True if strictly decreasing, otherwise false.
    \param   x   A vector.
*/
bool is_decreasing( ConstVectorView   x )
{
  if( x.nelem() > 1 )
    {
      for( Index i=1; i<x.nelem(); i++ )
        {
          if( x[i] >= x[i-1] )
            return false;
        }
    }
  return true;
}


//! Checks if a square matrix is singular.
/*! 
    If one row of a matrix has only 0 values the matrix is singular.

    Due to numerical inaccuracies the values can deviate from 0. 
    The value for the precision is defined in the file *logic.cc*.

    \return      True if matrix is singular.
    \param   A   A square matrix.
*/
bool is_singular( ConstMatrixView A )
{
  assert( A.nrows() == A.ncols() );
  Numeric temp = 0.;
    
  for( Index i=0; i<A.nrows(); i++)
    {
      Numeric big = 0.;
      for( Index j=0; j<A.nrows(); j++)
        {
          if ((temp = fabs(A(i,j))) > big)
            big = temp;
        }
      // Due to numerical precision the values can deviate from 0.0
      if (big < precision)
        {
        throw runtime_error ("Matrix is singular.");
        return true;
        }
    }
  return false;
}


//! Checks if a square matrix is diagonal.
/*! 
    If one off diagonal element is nonzero the function returns false.

    Due to numerical inaccuracies the values can deviate from 0. 
    The value for the precision is defined in the file *logic.cc*.

    \return      True if matrix is diagonal.
    \param   A   A square matrix.
*/
bool is_diagonal( ConstMatrixView A )
{
 assert( A.nrows() == A.ncols() );
 
 for( Index i=1; i<A.ncols(); i++ )
        {
          for( Index j=0; j<i; j++ )
            {
              if( fabs(A(i,j)) > precision ||  
                  fabs(A(j,i)) > precision )
                return false;
            }
        }
 return true;
}
