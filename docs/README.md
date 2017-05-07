# Sumary

Aplicació web per a la socialització de llibres

Angel Galindo Muñoz, zoquero@gmail.com

Maig de 2017

# Funcionalitats

* S'integra amb La Net de Fundació Jesuïtes Educació: Pot accedir a les dades de sessió dels usuari que prèviament hagin iniciant sessió a *La Net*
* Permet publicar-hi anuncis de llibres
* Permet buscar anuncis de llibres publicats
* Permet mostrar interès en llibres publicats

# Instal·lació

## Carpeta web

* Es copia al servidor la carpeta "**`www`**" del zip entregat

Exemple:
```
$ unzip liber.zip
$ mv www/ /var/www/liber
```


* S'hi estableixen **permisos** per a que el procés del servei web pugui:
    * **Llegir** i **executar** tots els seus fitxers 
    * **Escriure** sobre les seves subcarpetes
        * **`uploadstore/`**
        * **`uploadtmp/`**

Exemple:
```
$ chown -R www-data /var/www/liber/
$ chmod -R 0500 /var/www/liber/
$ chmod -R 0700 /var/www/liber/uploadstore/
$ chmod -R 0700 /var/www/liber/uploadtmp/
```

* Es configura la publicació de la carpeta web per a que
    * No es puguin llegir els fitxers "**`*.inc`**"
    * No es puguin accedir a la carpeta "**`www/lib`**"

Per exemple pot fer-se així, utilitzant el fitxer "**`priv/liber_site.conf`**" :
```
$ ln -s /var/www/liber/priv/liber_site.conf /etc/apache2/sites-enabled/    ## Ajusta prèviament els paths que hi trobaràs dins
```

## Base de dades

* S'escull un servidor de bases de dades MySql (seu FQDN), nom de base de dades, nom d'usuari i seu password i s'estableixen respectivament sobre les següents variables del fitxer **`www/conf.inc`**
    * **`$GLOBALS['dbHostname']`**
    * **`$GLOBALS['dbName']`**
    * **`$GLOBALS['dbUsername']`**
    * **`$GLOBALS['dbPassword']`**

* Es crea la base de dades, l'usuari i se li donen permisos:

```
# (corregeix servidor, usuari,i db)
$ mysql -u root -p
mysql> CREATE DATABASE liberdb;
mysql> CREATE USER 'liberdbuser'@'localhost' IDENTIFIED BY 'password';
mysql> GRANT CREATE, DELETE, INSERT, SELECT, UPDATE PRIVILEGES
             ON liberdb TO 'liberdbuser'@'localhost';
mysql> GRANT CREATE, DELETE, INSERT, SELECT, UPDATE PRIVILEGES
             ON liberdb.* TO 'liberdbuser'@'localhost';
mysql> flush privileges;
```

* Es creen les taules i es carreguen les dades inicials a partir dels scripts **`priv/ddl.sql`** i **`priv/dades.sql`**. Exemple:
```
# (corregeix usuari, password i db)
$ mysql -u liberdbuser -ppassword liberdb < priv/ddl.sql
$ mysql -u liberdbuser -ppassword liberdb < priv/dades.sql
```

## Test

Es provar l'aplicació accedint-hi amb un navegador. És autoexplicativa. Per exemple pot pujar-s'hi un anunci de prova i després esborrar-lo.

