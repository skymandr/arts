/* Copyright (C) 2002-2012
   Patrick Eriksson <Patrick.Eriksson@chalmers.se>
   Stefan Buehler   <sbuehler@ltu.se>
                            
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
  \file   m_general.cc
  \author Patrick Eriksson <Patrick.Eriksson@chalmers.se>
  \date   2002-05-08 

  \brief  Workspace functions of a general and overall character.

  This file is for general functions that do not fit in any other "m_"-file.

  These functions are listed in the doxygen documentation as entries of the
  file auto_md.h.
*/



/*===========================================================================
  === External declarations
  ===========================================================================*/

#include "arts.h"

#include <stdexcept>
#ifdef TIME_SUPPORT
#include <unistd.h>
#endif

#include "m_general.h"
#include "array.h"
#include "check_input.h"
#include "messages.h"
#include "mystring.h"

#include "math_funcs.h"
#include "make_vector.h"
#include "wsv_aux.h"

#include "auto_md.h"
#include "geodetic.h"
#include "workspace_ng.h"

/*===========================================================================
  === The functions (in alphabetical order)
  ===========================================================================*/

/* Workspace method: Doxygen documentation will be auto-generated */
void INCLUDE(const Verbosity&)
{
}


/* Workspace method: Doxygen documentation will be auto-generated */
void Print(Workspace& ws _U_,
           // WS Generic Input:
           const Agenda&   x,
           // Keywords:
           const Index&    level,
           const Verbosity& verbosity)
{
  ostringstream os;
  os << "     " << x << "\n";
  CREATE_OUTS;
  SWITCH_OUTPUT (level, os.str ());
}


/* Workspace method: Doxygen documentation will be auto-generated */
void Print(// WS Generic Input:
           const ArrayOfGridPos& x,
           // Keywords:
           const Index&          level,
           const Verbosity&      verbosity)
{
  ostringstream os;
  for( Index i=0; i<x.nelem(); i++ )
    os << "     " << x[i].idx << "  " << x[i].fd[0] << "  " << x[i].fd[1]
         << "\n";
  CREATE_OUTS;
  SWITCH_OUTPUT (level, os.str ());
}


/* Workspace method: Doxygen documentation will be auto-generated */
void Print(// WS Generic Input:
           const ArrayOfIndex& x,
           // Keywords:
           const Index&        level,
           const Verbosity&    verbosity)
{
  ostringstream os;
  for( Index i=0; i<x.nelem(); i++ )
    os << x[i] << " ";
  CREATE_OUTS;
  SWITCH_OUTPUT (level, os.str () << '\n');
}


/* Workspace method: Doxygen documentation will be auto-generated */
void Print(// WS Generic Input:
           const ArrayOfString& x,
           // Keywords:
           const Index&         level,
           const Verbosity&     verbosity)
{
  ostringstream os;
  for( Index i=0; i<x.nelem(); i++ )
    os << x[i] << '\n';
  CREATE_OUTS;
  SWITCH_OUTPUT (level, os.str ());
}


/* Workspace method: Doxygen documentation will be auto-generated */
void
Print(// WS Generic Input:
      const Ppath&     x,
      // Keywords:
      const Index&     level,
      const Verbosity& verbosity)
{
  CREATE_OUTS;
  SWITCH_OUTPUT (level, "dim: ");
  Print( x.dim, level, verbosity );
  SWITCH_OUTPUT (level, "np: ");
  Print( x.np, level, verbosity );
  SWITCH_OUTPUT (level, "constant: ");
  Print( x.constant, level, verbosity );
  SWITCH_OUTPUT (level, "background: ");
  Print( x.background, level, verbosity );
  SWITCH_OUTPUT (level, "start_pos: ");
  Print( x.start_pos, level, verbosity );
  SWITCH_OUTPUT (level, "start_los: ");
  Print( x.start_los, level, verbosity );
  SWITCH_OUTPUT (level, "start_lstep: ");
  Print( x.start_lstep, level, verbosity );
  SWITCH_OUTPUT (level, "pos: ");
  Print( x.pos, level, verbosity );
  SWITCH_OUTPUT (level, "los: ");
  Print( x.los, level, verbosity );
  SWITCH_OUTPUT (level, "r: ");
  Print( x.r, level, verbosity );
  SWITCH_OUTPUT (level, "lstep: ");
  Print( x.lstep, level, verbosity );
  SWITCH_OUTPUT (level, "end_pos: ");
  Print( x.end_pos, level, verbosity );
  SWITCH_OUTPUT (level, "end_los: ");
  Print( x.end_los, level, verbosity );
  SWITCH_OUTPUT (level, "end_lstep: ");
  Print( x.end_lstep, level, verbosity );
  SWITCH_OUTPUT (level, "nreal: ");
  Print( x.nreal, level, verbosity );
  SWITCH_OUTPUT (level, "ngroup: ");
  Print( x.ngroup, level, verbosity );
  SWITCH_OUTPUT (level, "gp_p: ");
  Print( x.gp_p, level, verbosity );
  if( x.dim >= 2 )
    {
      SWITCH_OUTPUT (level, "gp_lat: ");
      Print( x.gp_lat, level, verbosity );
    }
  if( x.dim == 3 )
    {
      SWITCH_OUTPUT (level, "gp_lon: ");
      Print( x.gp_lon, level, verbosity );
    }
}


/* Workspace method: Doxygen documentation will be auto-generated */
void Print(// WS Generic Input:
           const ArrayOfPpath& x,
           // Keywords:
           const Index&        level,
           const Verbosity&    verbosity)
{
  CREATE_OUTS;
  for( Index i=0; i<x.nelem(); i++ )
    {
      ostringstream os;
      os << "Ppath element " << i << ": ";
      SWITCH_OUTPUT (level, os.str ());
      Print( x[i], level, verbosity );
    }
}


/* Workspace method: Doxygen documentation will be auto-generated */
void Print(// WS Generic Input:
           const Timer&   /* x */,
           // Keywords:
           const Index&   /* level */,
           const Verbosity&)
{
/*  ostringstream os;
  CREATE_OUTS;
  cout << "  *" << x_name <<"* =";
  SWITCH_OUTPUT (level, "  *" << x_name << "*:\n");
  for( Index i=0; i<x.nelem(); i++ )
    os << x[i] << '\n';
  SWITCH_OUTPUT (level, os.str ()); */
}


/* Workspace method: Doxygen documentation will be auto-generated */
void PrintWorkspace(// Workspace reference
                    Workspace& ws,
                    // Keywords:
                    const Index&     only_allocated,
                    const Index&     level,
                    const Verbosity& verbosity)
{
  ostringstream os;

  if (only_allocated)
    os << "  Allocated workspace variables: \n";
  else
    os << "  Workspace variables: \n";
  for (Index i = 0; i < ws.nelem(); i++)
    {
      if (!only_allocated)
        {
          os << "    ";
          PrintWsvName (os, i);
          if (ws.is_initialized(i)) os << "+";
          os << "\n";
        }
      else if (ws.is_initialized(i))
        {
          os << "    ";
          PrintWsvName (os, i);
          os << "\n";
        }
    }
  CREATE_OUTS;
  SWITCH_OUTPUT (level, os.str ());
}


/* Workspace method: Doxygen documentation will be auto-generated */
#ifdef TIME_SUPPORT
void
timerStart (// WS Output
            Timer& starttime,
            const Verbosity&)
{
  if ((starttime.realtime = times (&starttime.cputime)) == (clock_t)-1)
    throw runtime_error ("Timer error: Unable to get current CPU time");
}
#else
void
timerStart (// WS Output
            Timer& /*starttime*/,
            const Verbosity&)
{
  throw runtime_error ("Timer error: ARTS was compiled without POSIX support, thus timer\nfunctions are not available.");
}
#endif


/* Workspace method: Doxygen documentation will be auto-generated */
#ifdef TIME_SUPPORT
void
timerStop (// WS Input
           const Timer& starttime,
           const Verbosity& verbosity)
{
  CREATE_OUT1;
  
  Timer endtime;
  static long clktck = 0;

  if (clktck == 0)
    if ((clktck = sysconf (_SC_CLK_TCK)) < 0)
      throw runtime_error ("Timer error: Unable to determine CPU clock ticks");

  if ((endtime.realtime = times (&endtime.cputime)) == (clock_t)-1)
    throw runtime_error ("Timer error: Unable to get current CPU time");

  // FIXME: out1 does not have setf, but we need to set it here
  cout.setf (ios::showpoint | ios::fixed);

  out1 << "  * CPU time  total: " << setprecision (2)
    << (Numeric)((endtime.cputime.tms_stime - starttime.cputime.tms_stime)
        + (endtime.cputime.tms_utime - starttime.cputime.tms_utime))
    / (Numeric)clktck;

  out1 << "  user: " << setprecision (2)
    << (Numeric)(endtime.cputime.tms_utime - starttime.cputime.tms_utime)
    / (Numeric)clktck;

  out1 << "  system: " << setprecision (2)
    << (Numeric)(endtime.cputime.tms_stime - starttime.cputime.tms_stime)
    / (Numeric)clktck;

  out1 << "\n               real: " << setprecision (2)
    << (Numeric)(endtime.realtime - starttime.realtime) / (Numeric)clktck;

  out1 << "  " << setprecision (2)
    << (Numeric)((endtime.cputime.tms_stime - starttime.cputime.tms_stime)
        + (endtime.cputime.tms_utime - starttime.cputime.tms_utime))
    / (Numeric)(endtime.realtime - starttime.realtime) * 100.
    << "%CPU\n";
}
#else
void
timerStop (// WS Input
           const Timer&,
           const Verbosity&)
{
  throw runtime_error ("Timer error: ARTS was compiled without POSIX support, thus timer\nfunctions are not available.");
}
#endif

/* Workspace method: Doxygen documentation will be auto-generated */
void Error(const String& msg, const Verbosity& verbosity)
{
  CREATE_OUT0;
  out0 << msg << "\n";
  arts_exit();
}



/* Workspace method: Doxygen documentation will be auto-generated */
void Exit(const Verbosity& verbosity)
{
  CREATE_OUT1;
  out1 << "  Forced exit.\n";
  arts_exit (EXIT_SUCCESS);
}



/* Workspace method: Doxygen documentation will be auto-generated */
void Test(const Verbosity& verbosity)
{
  // This function can be used to test stuff.
  // Feel free to change and test things

  Exit( verbosity );
}


/* Workspace method: Doxygen documentation will be auto-generated */
void verbosityInit(// WS Output:
                   Verbosity& verbosity)
{
  extern Verbosity verbosity_at_launch;
  
  verbosity.set_screen_verbosity(verbosity_at_launch.get_screen_verbosity());
  verbosity.set_agenda_verbosity(verbosity_at_launch.get_agenda_verbosity());
  verbosity.set_file_verbosity(verbosity_at_launch.get_file_verbosity());
}


/* Workspace method: Doxygen documentation will be auto-generated */
void verbositySet(// WS Output:
                  Verbosity& verbosity,
                  // WS Generic Input:
                  const Index& agenda,
                  const Index& screen,
                  const Index& file)
{
  verbosity.set_agenda_verbosity(agenda);
  verbosity.set_screen_verbosity(screen);
  verbosity.set_file_verbosity(file);
}


/* Workspace method: Doxygen documentation will be auto-generated */
void verbositySetAgenda(// WS Output:
                        Verbosity& verbosity,
                        // WS Generic Input:
                        const Index& level)
{
  verbosity.set_agenda_verbosity(level);
}


/* Workspace method: Doxygen documentation will be auto-generated */
void verbositySetFile(// WS Output:
                      Verbosity& verbosity,
                      // WS Generic Input:
                      const Index& level)
{
  verbosity.set_file_verbosity(level);
}


/* Workspace method: Doxygen documentation will be auto-generated */
void verbositySetScreen(// WS Output:
                        Verbosity& verbosity,
                        // WS Generic Input:
                        const Index& level)
{
  verbosity.set_screen_verbosity(level);
}
