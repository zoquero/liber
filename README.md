# Sumari

Aplicació web feta en PHP que té com a finalitat la **reutilització de llibres de l'escola Jesuïtes Bellvitge**. Permet que pares i mares cedeixin els seus llibres en desús a d'altres famílies que ho desitjin. És una iniciativa de l'**AMPA** que va en la línia del [consum col·laboratiu](https://ca.wikipedia.org/w/index.php?title=Consum_col%C2%B7laboratiu&oldid=18443245), convidant a fer-ne un ús desinteressat i totalment solidari amb d'altres famílies de l'escola.

S'ha desplegat sobre servidors de la plataforma informàtica de la [Fundació Jesuïtes Educació](http://www.fje.edu), s'ha integrat amb el seu Single-Sign-On (el de *la NET*) i utilitza el seu servei d'enviament de correus. S'ha dissenyat tenint en compte la privacitat. No comparteix cap dada personal de qui publica llibres, simplement facilita que els interessats en llibres puguin enviar les dades de contacte que considerin als anunciants.

És software lliure, es distribueix amb llicència GPLv3 i pots trobar el seu codi font a [GitHub](https://github.com/zoquero/liber).

Un agraïment molt especial a **Joan Gallifa** del departament de *Sistemes de la informació i comunicació de la Fundació Jesuïtes Educació* que ens va facilitar l'ús dels seus recursos informàtics.

Angel Galindo Muñoz, zoquero *at* gmail.com

Maig de 2017

# Funcionalitats

* S'integra amb La Net de Fundació Jesuïtes Educació: Pot accedir a les dades de sessió dels usuari que prèviament hagin iniciant sessió a *La Net*
* Permet publicar-hi anuncis de llibres
* Permet buscar anuncis de llibres publicats
* Permet mostrar interès en llibres publicats
* Permet registrar-se per ser notificat sobre nous llibres

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

Nota: Ara només suport MySQL. Podria suportar d'altres bases de dades com PostgreSQL amb canvis menors.

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

## Cron
Cal aconseguir que un cop al dia s'executi l'script **`bin/send_notifs.php`** per a enviar correus sobre els nous llibres anunciats. Aquests correus només s'envien als usuaris que s'han registrat per ser notificat sobre nous llibres de tal curs.

Pot fer-se afegint una entrada com aquesta al cron. Pot fer-se amb qualsevol usuari que tingui permís de lectura sobre els fitxers de l'aplicació:

```
0 19 * * * /www/liber/bin/send_notifs.php \\
           >> /www/liber/bin/send_notifs.out 2>&1
```

## Test

Es provar l'aplicació accedint-hi amb un navegador. És autoexplicativa. Per exemple pot pujar-s'hi un anunci de prova i després esborrar-lo.

# Desinstal·lació
Recorda que la configuració és a **`www/conf.inc`**

* Esborra les carpetes
    * **`uploadstore/`**
    * **`uploadtmp/`**
* Esborra la carpeta del site del sistema de fitxers
* Esborra la configuració del site al servidor web
* Esborra l'entrada al cron
* Esborra la base de dades
