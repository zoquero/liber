<?php

require_once("conf.inc");

/*
TO_DO : Backup authorization
	if(! isAdmin()) {
		die("Just for admins");
	}
*/
	$filename = $GLOBALS['dbName'] . "-" . date("Ymd-Gis") . ".sql.gz";
	$mime = "application/x-gzip";
	header( "Content-Type: " . $mime );
	header( 'Content-Disposition: attachment; filename="' . $filename . '"' );
	$cmd = "mysqldump --opt -u $GLOBALS[dbUsername] --password=$GLOBALS[dbPassword] -h $GLOBALS[dbHostname] $GLOBALS[dbName] | gzip --best";   
	passthru( $cmd );
	exit(0);
?>
