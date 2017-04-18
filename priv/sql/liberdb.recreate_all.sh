#!/bin/bash
cd "$(dirname "$0")"
echo "Introdueix primer dos cops les credencials de 'root' per esborrar i re-crear la base de dades, usuari i grants:"
cat ./liberdb.00.drop_database_delete_user.sql | mysql -u root -p
cat ./liberdb.01.create_database_i_grants.sql | mysql -u root -p
echo "Introdueix ara dos cops les credencials de 'liberdbuser' per crear taules i dades:"
cat ./liberdb.02.ddl | mysql liberdb -u liberdbuser -p 
cat ./liberdb.03.test_data.sql | mysql liberdb -u liberdbuser -p 
