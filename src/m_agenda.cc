/* Copyright (C) 2002 Stefan Buehler <sbuehler@uni-bremen.de>

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
  \file   m_agenda.cc
  \author Stefan Buehler <sbuehler@uni-bremen.de>
  \date   Fri Mar 15 09:12:46 2002
  
  \brief  Workspace methods for Agenda.
*/

#include <map>
#include "agenda_class.h"
#include "wsv_aux.h"
#include "agenda_record.h"

void AgendaSet(// WS Generic Output:
               Agenda& output_agenda,
               // WS Generic Output Names:
               const String& agenda_name,
               // Agenda from controlfile:
               const Agenda& input_agenda)
{
  // Make external data visible
  extern const Array<WsvRecord> wsv_data;
  extern const Array<AgRecord>  agenda_data;
  extern const std::map<String, Index> AgendaMap;

  output_agenda.resize(input_agenda.nelem());
  output_agenda = input_agenda;
  output_agenda.set_name(agenda_name);

  // First we have to find the lookup information for this agenda. We
  // use AgendaMap for this.

  map<String, Index>::const_iterator mi =
    AgendaMap.find( output_agenda.name() );

  // Find return end() if the string is not found. This means that the
  // lookup data for this agenda is missing!
  assert( mi != AgendaMap.end() );

  const AgRecord& this_data = agenda_data[mi->second];

  // Ok, we have the lookup data now.

  // Check that the output produced by the actual methods in the
  // agenda corresponds to what is desired in the lookup data:
  for ( Index i=0; i<this_data.Output().nelem(); ++i )
    {
      // The WSV for which to check:
      Index this_wsv = this_data.Output()[i];

      if ( !output_agenda.is_output(this_wsv) )
        {
          ostringstream os;
          os << "The agenda " << output_agenda.name()
             << " must generate the output WSV "
             << wsv_data[this_wsv] << ",\n"
             << "but it does not. It only generates:\n";
          for ( Index j=0; j<wsv_data.nelem(); ++j )
            if ( output_agenda.is_output(j) )
              os << wsv_data[j].Name() << "\n";
          throw runtime_error (os.str());
        }
    }

  // Check that the input used by the actual methods in the
  // agenda corresponds to what is desired in the lookup data:
  for ( Index i=0; i<this_data.Input().nelem(); ++i )
    {
      // The WSV for which to check:
      Index this_wsv = this_data.Input()[i];

      if ( !output_agenda.is_input(this_wsv) )
        {
          ostringstream os;
          os << "The agenda " << output_agenda.name()
             << " must use the input WSV "
             << wsv_data[this_wsv].Name() << ",\n"
             << "but it does not. It only uses:\n";
          for ( Index j=0; j<wsv_data.nelem(); ++j )
            if ( output_agenda.is_input(j) )
              os << wsv_data[j].Name() << "\n";
          throw runtime_error (os.str());
        }
    }
}

void Main(// Agenda from controlfile:
          const Agenda& input_agenda)
{
  input_agenda.execute();
}
