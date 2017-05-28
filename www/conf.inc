<?php

$GLOBALS['debug']          = FALSE;  // Mostrar info pel troubleshooting
$GLOBALS['mockToken']      = FALSE;  // Fixar el token per codi, no cookie 
$GLOBALS['mockSess']       = TRUE;  // Recuperar sessió de fitxer
$GLOBALS['mockInjectSess'] = FALSE;  // Injectar cookie de sessió
                                     // Estableix-la llavors a
                                     // $GLOBALS['SESS_FJEAPI_MOCKING_COOKIE_VALUE']

/* Segons a dormir el batch després d'enviar
 * cada correu amb notificacions sobre anuncis.
 */
$GLOBALS['SLEEP_SEC_BETWEEN_NOTIFICATIONS'] = 5;

/*** mysql hostname ***/
$GLOBALS['dbHostname'] 	= 'localhost';
/*** mysql username ***/
$GLOBALS['dbUsername'] 	= 'liberdbuser';
/*** mysql password ***/
$GLOBALS['dbPassword'] 	= 'password';
/*** mysql database name ***/
$GLOBALS['dbName'] 		= 'liberdb';

/*** Subcarpeta temporal dins de la carpeta web,
  ** per recepció i compressió d'imatges,
  ** previ a l'acceptació i emmagatzemament.
  ** Qualsevol fitxer que contingui
  ** i que no hagi estat modificat en menys d'1 hora pot ser esborrat.
  ***/
$GLOBALS['uploadFolderTmp'] 		= 'uploadtmp';
/*** Subcarpeta dins de la carpeta web, on es guarden les imatges dels anuncis. ***/
$GLOBALS['uploadFolderStore'] 		= 'uploadstore';
/*** Màxim temps (en segons) que pot estar un fitxer a la carpeta uploadFolderTmp mentres no és confirmat.
 * Si el temps es supera serà borrat en següents file uploads. */
$GLOBALS['maxTmpImageLifetime'] = 600;

/*** Qualitat per les imatges redimensionades en percentatge ([0..100]) ***/
$GLOBALS['imageCompressionPercent'] = 60;
/*** Màxima dimensió d'imatge 1 ***/
$GLOBALS['maxImageDimension1'] 		= 640;
/*** Màxima dimensió d'imatge 2 ***/
$GLOBALS['maxImageDimension2'] 		= 480;
/*** Dimensió de thumbnail 1 ***/
$GLOBALS['thumbnailDimension1'] 	= 200;
/*** Dimensió de thumbnail 2 ***/
$GLOBALS['thumbnailDimension2'] 	= 150;
/*** Prefix per les imatges */
$GLOBALS['imageFilenamePrefix'] = "LiBeR_ImG_";
/*** Sufix a concatenar la nom dels thumbnails. */
$GLOBALS['thumbFilenameSufix'] = ".thumb";
define("IMAGE_PORTRAIT_OK", 		1);
define("IMAGE_PORTRAIT_TO_RESIZE",	2);
define("IMAGE_LANDSCAPE_OK", 		3);
define("IMAGE_LANDSCAPE_TO_RESIZE",	4);


/*** Llista separada per espais dels administradors de l'aplicació. */
$GLOBALS['appAdmins'] = array("zoquero2@gmail.com", "ampa.joan23@fje.edu");

$GLOBALS['WEB_AMPA'] = "http://bit.ly/ampajoan23";

/*** Títol head de les pàgines */
$GLOBALS['htmlHeadTitle'] = "Liber: Socialització de llibres impulsada per l'AMPA de Joan XXIII";
/*** Títol body de les pàgines */
$GLOBALS['htmlBodyTitle'] = "Liber: Socialització de llibres";


/*** FJEAPI */
$GLOBALS['MAIL_LIB'] = "FJEAPI";
// $GLOBALS['MAIL_FJEAPI_URL']='http://srv.fje.edu/api/conNETctor.php/enviaInformacio';
$GLOBALS['MAIL_FJEAPI_HTTP_HOST']='apis.net.fje.edu';
$GLOBALS['MAIL_FJEAPI_HTTP_PORT']='80';
$GLOBALS['MAIL_FJEAPI_HTTP_PATH']='/api/conNETctor.php/enviaInformacio';
$GLOBALS['MAIL_FJEAPI_MAX_BUFF']='8192';
$GLOBALS['MAIL_FJEAPI_MAX_RESPONSE_SIZE']='102400';
$GLOBALS['MAIL_FJEAPI_OKRESPONSE_SIMPLIFIED']='{Resultat:Ok}';

$GLOBALS['SESS_FJEAPI_BASE_URL']='http://apis.net.fje.edu/api/conNETctor.php/validatokenCookie';
$GLOBALS['SESS_FJEAPI_COOKIE_NAME']='tokenNet';
// Path with mocking user data, relative to ROOT_PATH
$GLOBALS['SESS_FJEAPI_MOCKING_DATA_FILE']="../priv/sess_sample/fje_validatokenCookie.DUMMY.json";
$GLOBALS['SESS_FJEAPI_MOCKING_COOKIE_VALUE']='BE480C0C-AF79-4BFD-AFBF-E5472BC0DE5A';

/*** Timeout de connexió fent fsockopen per enviar correu via L'API de FJE */
$GLOBALS['SOCKET_CONN_TIMEOUT']=30;
/*** Timeout llegint i escrivint del socket per enviar correu via L'API de FJE */
$GLOBALS['SOCKET_RW_TIMEOUT']=30;

/**
  * SMTP debugging.
  * 0 = off (for production use)
  * 1 = client messages
  * 2 = client and server messages
  */
$GLOBALS['MAIL_DEBUG'] 			= 0;
/** Mail host */
$GLOBALS['MAIL_HOST'] 			= 'smtp.gmail.com';
/** Mail port, 587 per SMTPS de Gmail */
$GLOBALS['MAIL_PORT'] 			= 587;
/** True per SMTP Auth*/
$GLOBALS['MAIL_SMTPAUTH'] 		= true;
/** username del servei SMTP.
 * 
  * Nota: si el compte a utilitzar és de Google cal llegir el document
  * https://support.google.com/accounts/answer/6010255?hl=es
  * 
  * i errors posteriors: https://support.google.com/mail/answer/14257?rd=1 
  * "Hemos detectado actividad inusual en la cuenta a la que estás intentando acceder.
  * Para continuar, sigue las instrucciones siguientes."
  */
$GLOBALS['MAIL_USERNAME'] 		  = 'none@gmail.com';
$GLOBALS['MAIL_PASSWORD'] 		  = 'password';
/** 'tls' per SMTPS de Gmail */
$GLOBALS['MAIL_SMTPSECURE'] 	  = 'tls';
$GLOBALS['MAIL_FROM'] 			  = 'none@gmail.com';
$GLOBALS['MAIL_FROM_NAME']		  = 'AMPA Joan XXIII (no responguis)';
$GLOBALS['MAIL_REPLYTO'] 		  = 'none@gmail.com';
$GLOBALS['MAIL_REPLYTO_NAME']	  = 'AMPA Joan XXIII (no responguis)';
$GLOBALS['MAIL_SUBJECT']		  = 'Algú està interessat en un llibre que has anunciat';
$GLOBALS['MAIL_SUBJECT_NOTIFYME'] = "S'ha publicat un anunci de llibre que potser t'interessi";
$GLOBALS['LIBER_URL']             = 'http://liber.net.fje.edu/';

define("ACTION_NONE",                     0);
define("ACTION_SET_DUMMY_USERNAME",       1);
define("ACTION_NEW_AD_FORM",              2);
define("ACTION_SEARCH_BY_ISBN",           3);
define("ACTION_SEARCH_BY_KEYWORDS",       4);
define("ACTION_NEW_AD_PREVIEW",           5);
define("ACTION_SHOW_MY_ADS",              6);
define("ACTION_SHOW_ALL_ADS",             7);
define("ACTION_DELETE_ADD",               8);
define("ACTION_BE_INTERESTED_IN_ADD",     9);
define("ACTION_BE_INTERESTED_IN_ADD_DO", 10);
// define("ACTION_BACKUP",                  11);
define("ACTION_RESTORE",                 12);
define("ACTION_SEE_NOTIFICATIONS",       13);
define("ACTION_SEARCH_BY_GRADE",         14);
define("ACTION_SHOW_LOGS",               15);
define("ACTION_NOTIFYME_SHOW",           16);
define("ACTION_NOTIFYME_DO",             17);
define("ACTION_NOTIFYME_MAIL",           18);
define("ACTION_ABOUT",                   19);

define("AD_STATUS_PUBLISHED", 1);
define("AD_STATUS_GRANTED",   2);

?>