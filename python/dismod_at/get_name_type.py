# $Id:$
# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-24 Bradley M. Bell
# ----------------------------------------------------------------------------
# {xrst_begin get_name_type}
# {xrst_spell
#     tbl
# }
# {xrst_comment_ch #}
#
# Get Column Names and Types in a Table
# #####################################
#
# Syntax
# ******
# ( *col_name* , *col_type* )
#
#     = ``dismod_at.get_name_type`` ( *connection* , *tbl_name* )
#
# connection
# **********
# is a :ref:`create_connection@connection` for this database.
#
# table_name
# **********
# is a ``str`` that specifies the name of the table.
#
# col_name
# ********
# is a list of strings containing the column names in the same order as they
# appear in the table.
#
# col_type
# ********
# is a list of strings containing the column types in the same order as
# the columns appear in the table.
# The possible values for the column types are
# ``integer`` , ``real`` , ``text`` , or
# ``integer primary key`` .
# Note that the types in the database are converted to lower case before
# being returned in *col_type* .
#
# Primary Key
# ***********
# This routine assumes the primary key is an integer,  corresponds
# to the first column, and has name *tbl_name* _ ``id`` .
# {xrst_toc_hidden
#    example/table/get_name_type.py
# }
# Example
# *******
# The file :ref:`get_name_type.py-name` is an example use of
# ``get_name_type`` .
#
# {xrst_end get_name_type}
# ---------------------------------------------------------------------------
import dismod_at
def get_name_type(connection, tbl_name) :
   #
   # database
   database = dismod_at.connection_file(connection)
   #
   # cursor
   cursor    = connection.cursor()
   #
   # check if table exists
   cmd     = "select * from sqlite_master where type='table' AND name="
   cmd    += "'" + tbl_name + "';"
   info    = cursor.execute(cmd).fetchall()
   if len(info) == 0 :
      msg = f'get_name_type: table {tbl_name} does not exist in {database}'
      assert False, msg
   #
   # pragma table_info for this table
   cmd       = 'pragma table_info(' + tbl_name + ');'
   found_pk  = False
   col_name  = list()
   col_type  = list()
   #
   # current column id
   cid       = 0
   #
   # row
   for row in cursor.execute(cmd) :
      assert cid == row[0]
      #
      col_name.append(row[1])
      col_type.append( row[2].lower() )
      pk            = row[5]
      if cid == 0 :
         if pk != 1 :
            msg     = f'{tbl_name} table in {database}'
            msg    += '\nfirst column not the primary key'
            assert False, msg
         assert found_pk == False
         assert col_type[cid] == 'integer'
         assert col_name[cid] == (tbl_name + '_id')
         col_type[cid]  =  'integer primary key'
         found_ok       = True
      else :
         if pk != 0 :
            msg     = f'{tbl_name} table in {database}'
            msg    += '\n muiltiple columns in primary key'
            assert False, msg
      cid += 1
   return (col_name, col_type)
