<?php

require_once("conf.inc");
require_once("lib/PHPMailer/PHPMailerAutoload.php");

function myErrorHandler($errno, $errstr, $errfile, $errline) {
	if (!(error_reporting() & $errno)) {
		// Codi no inclòs a l'error_reporting
		return;
	}

    switch ($errno) {
    case E_USER_ERROR:
        echo "<table border=\"1\"><tr><td bgcolor=\"#FFC0C0\">"
			. "<b>Error:</b> [$errno] $errstr<br>"
			. "Línia $errline de $errfile<br>"
			. "(abortem l'execuci&oacute;)"
			. "</td></tr></table>";
        exit(1);
        break;
    case E_USER_WARNING:
        echo "<table border=\"1\"><tr><td bgcolor=\"#ffdead\">"
			. "<b>Av&iacute;s:</b> [$errno] $errstr<br>"
			. "Línia $errline de $errfile<br>"
			. "</td></tr></table>";
        break;
    case E_USER_NOTICE:
        echo "<table border=\"1\"><tr><td bgcolor=\"#fffacd\">"
			. "<b>Nota:</b> [$errno] $errstr<br>"
			. "Línia $errline de $errfile<br>"
			. "</td></tr></table>";
        break;
    case E_NOTICE:
		echo "<table border=\"1\"><tr><td bgcolor=\"#fffacd\">"
        	. "<b>Nota:</b> [$errno] $errstr<br>"
        	. "Línia $errline de $errfile<br>"
        	. "</td></tr></table>";
        break;    
    
    default:
        echo "<table border=\"1\"><tr><td bgcolor=\"#FFC0C0\">"
        	. "<b>Error:</b> [$errno] $errstr<br>"
        	. "Línia $errline de $errfile<br>"
        	. "(abortem l'execuci&oacute;)"
			. "</td></tr></table>";
        	exit(1);
   		break;
    }
			
			
}

function echoErr() {
	echo "<table border=\"1\"><tr><td bgcolor=\"#FFC0C0\">"
			. "<b>Error:</b> [$errno] $errstr<br>"
			. "Línia $errline de $errfile<br>"
			. "</td></tr></table>";
}

function getUser($dao) {
	$user = $dao->getCurrentUser();
	if($user == NULL) {
		if($GLOBALS['mock']) {
		}
		else {
			
		}
		if($GLOBALS['mock']) {
			trigger_error("No us hem trobat a la base de dades de <b>socis de l'AMPA</b>.<br/>
				Podeu <a href= \"mailto:ampa.joan23@fje.edu\" >contactar amb l'AMPA</a> per corregir-ho.", E_USER_WARNING);
		}
		else {
			trigger_error("No us hem trobat a la base de dades de <b>socis de l'AMPA</b>.<br/>
				Podeu <a href= \"mailto:ampa.joan23@fje.edu\" >contactar amb l'AMPA</a> per corregir-ho.", E_USER_ERROR);
		}
	}
	return $user;
}

/**
 * Retorna un paràmetre enter rebut via POST de forma segura.
 * <p>Algunes recomanacions extretes de http://phpsec.org/projects/guide/</p>
 * @param String $paramName nom del paràmtre
 * @return int
 */
function getParamIntPost($paramName) {
	return filter_input(INPUT_POST, $paramName, FILTER_VALIDATE_INT);
}

/**
 * Retorna un paràmetre enter rebut via GET de forma segura
 * @param String $paramName nom del paràmtre
 * @return int
 */
function getParamIntGet($paramName) {
	return filter_input(INPUT_GET, $paramName, FILTER_VALIDATE_INT);
}

/**
 * Retorna un paràmetre rebut, primer ho prova via POST i si no via GET
 * @param String $paramName nom del paràmtre
 * @return int
 */
function getParamInt($paramName) {
	$r = getParamIntPost($paramName);
	if(is_int($r))
		return $r;
	else {
		$r = getParamIntGet($paramName);
		if(is_int($r))
			return $r;
	}
	return null;
}

/**
 * Retorna sanejat un paràmetre String rebut via POST de forma segura.
 * <p>Algunes recomanacions extretes de http://phpsec.org/projects/guide/</p>
 * @param String $paramName nom del paràmtre
 * @return String
 */
function getParamSanitizedStringPost($paramName) {
	// htmlentities(), strip_tags(), and utf8_decode()
	// filter_var($str, FILTER_SANITIZE_STRING);
	// return urlencode(filter_input(INPUT_POST, $paramName, FILTER_SANITIZE_ENCODED));

	// return strip_tags(stripslashes(filter_var($_POST[$paramName], FILTER_SANITIZE_STRING)));
	// $newString = preg_replace('/[^a-z0-9]/i', '_', $join);

	//$out = preg_replace('/[^a-zA-Z0-9 @\.]/i', '', $_POST[$paramName]);
	//return $out;

	//	if($_POST[$paramName] == NULL)
	if(empty($_POST[$paramName]))
		return NULL;

	$t = strip_tags(stripslashes(filter_var($_POST[$paramName], FILTER_SANITIZE_STRING)));
	return preg_replace('/[^a-zA-Z0-9À-ÿ @\n\._\-]/i', '', $t);
}


	/**
	 * Retorna sanejat un paràmetre String rebut via POST de forma segura.
	 * <p>Algunes recomanacions extretes de http://phpsec.org/projects/guide/</p>
	 * @param String $paramName nom del paràmtre
	 * @return String
	 */
function getParamSanitizedStringGet($paramName) {
	//	if($_GET[$paramName] == NULL)
	if(empty($_GET[$paramName]))
		return NULL;

	$t = strip_tags(stripslashes(filter_var($_GET[$paramName], FILTER_SANITIZE_STRING)));
	return preg_replace('/[^a-zA-Z0-9À-ÿ @\.]/i', '', $t);
}

	/**
	 * Retorna sanejat un paràmetre String rebut via POST o GET de forma segura.
	 * <p>Algunes recomanacions extretes de http://phpsec.org/projects/guide/</p>
	 * @param String $paramName nom del paràmtre
	 * @return String
	 */
function getParamSanitizedString($paramName) {
	$r = getParamSanitizedStringPost($paramName);
	if($r != NULL)
		return $r;
	else {
		$r = getParamSanitizedStringGet($paramName);
		if($r != NULL)
			return $r;
	}
	return NULL;
}



/**
 * Estableix el nom d'usuari a la sessió a partir de paràmetre POST, només pel desenvolupament.
 */
function setDummyUsername() {
	$_SESSION["dummyUsername"] = getParamSanitizedString('dummyUsername');
	echo "<p>Ara has esdevingut l'usuari " . $_SESSION["dummyUsername"] . "</p>";
}


/**
 * Escriu les capçaleres del cos, amb el menú d'opcions per a l'usuari.
 */
function showRegularBodyHeaders() {
?>
<body>
	<h1>
		<?php echo htmlentities($GLOBALS['htmlBodyTitle'], ENT_QUOTES, "UTF-8") ?>
	</h1>
<?php
}

function getEnabledGradesSelect($dao, $fGrade) {
	$grades = $dao->getEnabledGrades();
	$gradesSelect="<select name=\"grade\">\n";
	$gradesSelect.="  <option value=\"-1\">Escull un curs</option>\n";
	foreach ($grades as $aGrade) {
		if($fGrade != NULL && $fGrade->getId() == $aGrade->getId()) {
			$selected=" selected";
		}
		else {
			$selected="";
		}
		$gradesSelect.="  <option value=\"" . $aGrade->getId() . "\" $selected>" . $aGrade->getName() . "</option>\n";
	}
	$gradesSelect.="</select>\n";
	
	return $gradesSelect;
}

/**
 * Escriu el menú d'opcions per a l'usuari.
 */
function showRegularOptionsMenu($dao) {
	$gradesSelect = getEnabledGradesSelect($dao, NULL);
?>
	<table border="1"><tr><td bgcolor="#F0F0F0">
		<p>Opcions:</p>
		<ul>
			<li><a href="?actionId=<?php echo ACTION_NEW_AD_FORM ?>">
				Nou llibre en donaci&oacute;</a>
			</li>
			<li><a href="?actionId=<?php echo ACTION_SHOW_MY_ADS ?>">
				Meus llibres anunciats</a>
			</li>
			<li>
				<form action="?" method="post" accept-charset="utf-8">
					<input type="hidden" name="actionId"
						value="<?php echo ACTION_SEARCH_BY_GRADE ?>" />
					<?php echo $gradesSelect ?>
					<input type="submit" name="Envia"
						value="Mostra llibres de tal curs" />
				</form>
			</li>
			<li>
				<form action="?" method="post" accept-charset="utf-8">
					<input type="hidden" name="actionId"
						value="<?php echo ACTION_SEARCH_BY_ISBN ?>" />
					<input type="text" name="isbn" />
					<input type="submit" name="Envia" value="Cerca llibre per ISBN" />
				</form>
			</li>
			<li>
				<form action="?" method="post" accept-charset="utf-8">
					<input type="hidden" name="actionId"
						value="<?php echo ACTION_SEARCH_BY_KEYWORDS ?>" /> <input
						type="text" name="keywords" />
					<input type="submit" name="Envia"
						value="Cerca llibre per paraules clau" />
				</form>
			</li>
		</ul>
	</td></tr></table>
<?php
}




/**
 * Escriu el menú d'opcions per a l'usuari.
 */
function showAdminOptionsMenu() {
?>
	<table border="1"><tr><td bgcolor="#FFF0F0">
		<p>Opcions d'administrador:</p>
		<ul>
			<li><a href="dbbackup.php">
				Backup</a></li>
			<li><a href="?actionId=<?php echo ACTION_RESTORE ?>">
				Restore</a></li>
		</ul>
	</td></tr></table>
<?php
}


/**
 * Mostra el form que permet establir l'username al desenvolupador.
 *
 * <p>Mostra el form que permet establir l'username al desenvolupador,
 * quan no es disposa de servei de sessió.</p>
 */
function showDummyForm($user) {
?>
<table border="1"><tr><td bgcolor="#FFF0F0">
	<b>Sessi&oacute; <i>dummy</i></b> per a administradors:<br/>
	Ara actues com a [<b><?php echo htmlentities($user, ENT_QUOTES, "UTF-8"); ?></b>]<br/>

	<form action="?" method="post" accept-charset="utf-8">
		<input type="hidden" name="actionId" value="<?php echo ACTION_SET_DUMMY_USERNAME ?>" />
						
		<table border="0"><tr>
			<td>Accedeix com a aquest usuari:</td>
			<td>
				<input type="text" name="dummyUsername"
					value="<?php echo empty($_SESSION["dummyUsername"]) ? "" : $_SESSION["dummyUsername"] ?>" /></td>
			<td><input type="submit" name="Envia" value="Envia" /></td>
		</tr></table>
	
	</form>
</td></tr></table>
<?php
}

function showFooter() {
?>
</body>
</html>
<?php
}

function newAdFormShow($dao, $fIsbn, $fSummary, $fDescription, $fWhat, $fImageBasename, $fGrade) {
/*
 echo "fWhat        = " . $fWhat . "<br>\n";
echo "fIsbn        = " . $fIsbn . "<br>\n";
echo "fSummary     = " . $fSummary . "<br>\n";
echo "fDescription = " . $fDescription . "<br>\n";
*/

// ACTION_NEW_AD_FORM => ACTION_NEW_AD_PREVIEW => ACTION_NEW_AD_CONFIRM
//	if($GLOBALS["actionId"] == ACTION_NEW_AD_PREVIEW && $fWhat != "Envia")
if($GLOBALS["actionId"] == ACTION_NEW_AD_PREVIEW) {
	if($fWhat != "Edita") {
			$doReadOnly = TRUE;
	}
	else {
		$doReadOnly = FALSE;
	}
}
else {
	$doReadOnly = FALSE;
}

if($doReadOnly) {
	if($fGrade == NULL) {
		trigger_error("Error de programació: No s'ha rebut l'objecte amb el curs", E_USER_ERROR);
	}
	$gradesSelect=$fGrade->getName() . "\n<input type=\"hidden\" name=\"grade\" value=\"" . $fGrade->getId() . "\"/>\n";
}
else {
/*
 * <select name="dropdown">
<option value="Data Structures" selected>Data Structures</option>
<option value="Data Mining">Data Mining</option>
</select>
 */

/*
	$grades = $dao->getEnabledGrades();
	$gradesSelect="<select name=\"grade\">\n";
	$gradesSelect.="  <option value=\"-1\">Escull una opci&oacute;</option>\n";
	foreach ($grades as $aGrade) {
		if($fGrade != NULL && $fGrade->getId() == $aGrade->getId()) {
			$selected=" selected";
		}
		else {
			$selected="";
		}
		$gradesSelect.="  <option value=\"" . $aGrade->getId() . "\" $selected>" . $aGrade->getName() . "</option>\n";
	}
	$gradesSelect.="</select>\n";
*/
	$gradesSelect=getEnabledGradesSelect($dao, $fGrade);
}

?>
<hr />
<h2>Nou anunci de llibre</h2>
<form action="?" method="post" enctype="multipart/form-data" accept-charset="utf-8">
	<input type="hidden" name="actionId"
		value="<?php echo ACTION_NEW_AD_PREVIEW ?>" />
	<table border="1" bgcolor="#fafafa">
		<?php if(! $doReadOnly) { ?>
		<tr>
			<td>ISBN:</td>
			<td><input type="text" name="isbn" value="<?php echo $fIsbn ?>"
				size="13" maxlength="13" /></td>
		</tr>
		<tr>
			<td>Títol</td>
			<td><input type="text" name="summary" value="<?php echo $fSummary ?>"
				size="50" maxlength="199" /></td>
		</tr>
		<tr>
			<td>Descripció</td>
			<td><textarea name="description" rows="5" cols="50" maxlength="249"><?php echo $fDescription ?></textarea></td>
		</tr>
		<tr>
			<td>Curs</td>
			<td><?php echo $gradesSelect ?></td>
		</tr>
		<tr>
			<td>Imatge del llibre<br/>(opcional)</td>
			<td>

<?php
	if(! empty($fImageBasename)) {
?>
			<table border="0" cellpadding="10"><tr><td>
				<img src="<?php echo $GLOBALS['uploadFolderTmp'] . "/" . $fImageBasename . $GLOBALS['thumbFilenameSufix'] ?>" alt="Imatge del llibre 1" /> <br/>
			</td></tr></table>
			<input type="hidden" name="imageBasenameH" value="<?php echo $fImageBasename ?>" />
<?php
	}
	else {
		echo "No s'ha adjuntat cap imatge del llibre<br/>\n";
	}
?>

			  <input type="file" name="imageBasenameF" id="imageBasenameF">
			</td>
		</tr>
		<tr>
			<td colspan="2">&nbsp;<input type="submit" name="what"
				value="Preview" />
			</td>
		</tr>
		<?php } else { ?>
		<tr>
			<td>ISBN:</td>
			<td><input type="text" name="isbn" value="<?php echo $fIsbn ?>"
				size="13" maxlength="13" readonly /></td>
		</tr>
		<tr>
			<td>Títol</td>
			<td><input type="text" name="summary" value="<?php echo $fSummary ?>"
				size="50" maxlength="199" readonly /></td>
		</tr>
		<tr>
			<td>Descripció</td>
			<td><textarea name="description" rows="5" cols="50" maxlength="249" readonly><?php echo $fDescription ?></textarea></td>
		</tr>
		<tr>
			<td>Curs</td>
			<td><?php echo $gradesSelect ?></td>
		</tr>
		<tr>
			<td>Imatge</td>
			<td>
<?php
	if(! empty($fImageBasename)) {
?>
			<table border="0" cellpadding="10"><tr><td>
				<img src="<?php echo $GLOBALS['uploadFolderTmp'] . "/" . $fImageBasename . $GLOBALS['thumbFilenameSufix'] ?>" alt="Imatge del llibre 2" /> <br/>
			</td></tr></table>
			<input type="hidden" name="imageBasenameH" value="<?php echo $fImageBasename ?>" />
<?php
	}
	else {
		echo "No s'ha adjuntat cap imatge del llibre<br/>\n";
	}
?>
			</td>
		</tr>

		<tr>
			<td colspan="2">&nbsp;<input type="submit" name="what" value="Edita" />
			</td>
		</tr>
		<tr>
			<td colspan="2">&nbsp;<input type="submit" name="what" value="Envia" />
			</td>
		</tr>
		<?php } ?>
	</table>
</form>
<hr />
<?php
}


/**
 * Esborra fitxers antics, comprimeix imatge i genera thumbnail.
 * 1) Rep el fitxer del que s'ha fet upload
 * 2) El comprimeix
 * 3) deixar el fitxer resultant a $GLOBALS['uploadFolderTmp'] donant-li un basename canònic
 * Si l'usuari ho accepta, en posterior hit ja es mourà cap a $GLOBALS['uploadFolderStore']
 * @param string basenamePrefix prefix pel nom de fitxer temporal 
 * @return String|NULL basename del fitxer dins de $GLOBALS['uploadFolderTmp']
 */
function compressAndSaveTmpImatge($basenamePrefix) {
	removeOldFiles($GLOBALS['uploadFolderTmp'], $GLOBALS['maxTmpImageLifetime']);
	
	$tmpFilename = getTmpFile($GLOBALS['uploadFolderTmp'], $basenamePrefix);
	$basename=basename($tmpFilename);
	if(! compressImatge($_FILES['imageBasenameF']['tmp_name'], $tmpFilename, $GLOBALS['maxImageDimension1'], $GLOBALS['maxImageDimension2'])) {
		trigger_error("Errors comprimint la imatge rebuda \"" . $_FILES['imageBasenameF']['name'] . "\"" , E_USER_WARNING);
		return NULL;
	}
	$tmpThumbnailFilename=$tmpFilename . $GLOBALS['thumbFilenameSufix'];
	if(! compressImatge($tmpFilename, $tmpThumbnailFilename, $GLOBALS['thumbnailDimension1'], $GLOBALS['thumbnailDimension2'])) {
		trigger_error("Errors fent el thumbnail de la imatge rebuda \"" . $_FILES['imageBasenameF']['name'] . "\"" , E_USER_WARNING);
		return NULL;
	}
	rename($tmpFilename, 			$GLOBALS['uploadFolderTmp'] . "/" . $basename);
	rename($tmpThumbnailFilename,	$GLOBALS['uploadFolderTmp'] . "/" . $basename . $GLOBALS['thumbFilenameSufix']);
	return $basename;
}

/**
 * Esborra fitxers de més de tants segons d'una carpeta
 * @param String $folder carpeta
 * @param int $seconds número segons
 */
function removeOldFiles($folder, $seconds) {
	$now   = time();
	// Obrim la carpeta
	if ($handle = opendir($folder)) {
		// recorrem carpeta
		while (false !== ($file = readdir($handle))) {
			$filePath = $folder. "/" .$file;
			// l'esborrem si cal
			if (is_file($filePath)) {
				$file_info = pathinfo($filePath);
				if (filemtime($filePath) < ($now - $seconds)) {
					unlink($filePath);
				}
			}
		}
	}	
}

/**
 * Comprimeix imatge
 * @param String $origFilename
 * @param String $destFilename
 * @param int $dim1 típicament $GLOBALS['maxImageDimension1']
 * @param int $dim2 típicamenet $GLOBALS['maxImageDimension2']
 * @return boolean TRUE si ok, FALSE i errors */
function compressImatge($origFilename, $destFilename, $dim1, $dim2) {
//echo "compressImatge: rebut origFilename = $origFilename i destFilename = $destFilename<br/>\n";

	$imageInfo = getimagesize($origFilename);
	$width=$imageInfo[0];
	$height=$imageInfo[1];

	if($width >= $height) {
		// landscape
		$newWidth=$dim1;
		$newHeight=$height*$newWidth/$width;
		
		if($newHeight > $dim2) {
			$newHeight=$dim2;
			$newWidth=$newHeight*$width/$height;
		}
	}
	else {
		// portrait
//		$newHeight=$dim2;
//		$newWidth=$newHeight*$width/$height;

//		$newWidth=$dim2;
//		$newHeight=$height*$newWidth/$width;

		$newWidth=$dim2;
		$newHeight=$height*$newWidth/$width;
		
		if($newHeight > $dim1) {
			$newHeight=$dim1;
			$newWidth=$newHeight*$width/$height;
		}
	}
	
	if ($imageInfo['mime'] == 'image/jpeg') {
		$origImage	= imagecreatefromjpeg($origFilename);
	}
	elseif ($imageInfo['mime'] == 'image/gif') {
		$origImage	= imagecreatefromgif($origFilename);
	}
	elseif ($imageInfo['mime'] == 'image/png') {
		$origImage	= imagecreatefrompng($origFilename);
	}
	else {
		trigger_error("Tipus d'imatge desconegut. Només suportem JPEG, GIF i PNG" , E_USER_WARNING);
		return FALSE;
	}
	
	$destImage	= imagecreatetruecolor($newWidth, $newHeight);
	imagecopyresized($destImage, $origImage, 0, 0, 0, 0, $newWidth, $newHeight, $imageInfo[0], $imageInfo[1]);
	imagejpeg($destImage, $destFilename, $GLOBALS['imageCompressionPercent'] );
	return TRUE;
}


/**
 * Crea un thumbnail d'imatge JPEG
 * @param String $origFilename
 * @param String $destFilename
 * @param int $dim1 típicament $GLOBALS['thumbnailDimension1']
 * @param int $dim2 típicamenet $GLOBALS['thumbnailDimension2']
 * @return boolean TRUE si ok, FALSE i errors
 */
function thumbFromJpeg($origFilename, $destFilename, $dim1, $dim2) {
	echo "thumbFromJpeg: rebut origFilename = $origFilename i destFilename = $destFilename<br/>\n";

	$imageInfo = getimagesize($origFilename);
	$width=$imageInfo[0];
	$height=$imageInfo[1];

	if($width >= $height) {
		// landscape
		$newWidth=$dim1;
		$newHeight=$height*$newWidth/$width;
	}
	else {
		// portrait
		$newHeight=$dim2;
		$newWidth=$newHeight*$width/$height;
	}

	if(! $imageInfo['mime'] == 'image/jpeg') {
		trigger_error("Error de programació, només fem thumbnails de JPEG" , E_USER_WARNING);
		return FALSE;
	}

	$destImage	= imagecreatetruecolor($newWidth, $newHeight);

	// trigger_error("executarem: imagecopyresized($destImage, $origImage, 0, 0, 0, 0, 640, 480, $imageInfo[0], $imageInfo[1])" , E_USER_WARNING);
	imagecopyresized($destImage, $origImage, 0, 0, 0, 0, $newWidth, $newHeight, $imageInfo[0], $imageInfo[1]);
	// trigger_error("imagecopyresized ha creat $destFilename a partir de $origFilename" , E_USER_WARNING);

	imagejpeg($destImage, $destFilename, $GLOBALS['imageCompressionPercent'] );
	return TRUE;
}


/**
 * crea dins de la carpeta un fitxer temporal buit prèviament no existent i en retorna la ruta
 * @param String $folder Carpeta dins de la que crear el fitxer
 * @param String $basenamePrefix prefix a donar al fitxer
 * @return NULL|string NULL si errors, basename creat si ok (no full path, compte).
 */
function getTmpFile($folder, $basenamePrefix) {
	$r = tempnam($folder, $basenamePrefix);
	return $r;
}



/*
 function newAdPostedFormShow() {
	$fIsbn=getParamSanitizedString('isbn');
$fSummary=getParamSanitizedString('summary');
$fDescription=getParamSanitizedString('description');
$fCorrectOrSend=getParamSanitizedString('CorrectOrSend');

?>
<h2>Verifica si les dades s&oacute;n correctes:</h2>
<form action="?" method="post" accept-charset="utf-8">
<input type="hidden" name="actionId" value="<?php echo ACTION_NEW_AD_CONFIRM ?>"/>
<table border="1" bgcolor="#fafafa">
<tr><td>ISBN:</td><td><input type="text" name="isbn" value="<?php echo $fIsbn ?>" size="13" maxlength="13" readonly/></td></tr>
<tr><td>Títol</td><td><input type="text" name="summary" value="<?php echo $fSummary ?>" size="50" maxlength="199" readonly/></td></tr>
<tr><td>Descripció</td><td><textarea name="description" rows="5" cols="50" maxlength="249" readonly><?php echo $fDescription ?></textarea></td></tr>
<tr><td colspan="2">&nbsp;<input type="submit" name="CorrectOrSend" value="Corregeix"/></td></tr>
<tr><td colspan="2">&nbsp;<input type="submit" name="CorrectOrSend" value="Envia"/></td></tr>
</table>
</form>

<?php
}
*/


function createAd($dao, $user, $fIsbn, $fSummary, $fDescription, $fImageBasename, $grade) {
	return $dao->createAd($user, $fIsbn, $fSummary, $fDescription, $fImageBasename, $grade);
}


function createInterest($dao, $user, $theAd, $fDescription) {
	return $dao->createInterest($user, $theAd, $fDescription);
}


// perror => trigger_error
// function perror($msg) {
// 	print "<table border=\"1\" bgcolor=\"#F0F0F0\"><tr><td><p style=\"color:red\"><b>$msg</b></p></td></tr></table>\n";
// }

/**
 * Escriu un missatge utilitzant un paràgraf HTML
 * @param String $msg El missatge
 */
function showMessage($msg) {
	print "<p>$msg</p>\n";
}

/**
 * Inicia sesió aplicant algun mecanisme per evitar session hijacking.
 * <p>Algunes recomanacions extretes de http://phpsec.org/projects/guide/</p>
 */
function safeSessionStart() {
	session_start();
	// anti session-hijacking
	if (isset($_SESSION['HTTP_USER_AGENT'])) {
		if ($_SESSION['HTTP_USER_AGENT'] != md5($_SERVER['HTTP_USER_AGENT'])) {
			trigger_error("Hi ha hagut algun problema accedint a la sessi&oacute; de l'usuari", E_USER_ERROR);
		}
	}
	else {
		$_SESSION['HTTP_USER_AGENT'] = md5($_SERVER['HTTP_USER_AGENT']);
	}
}

/**
 * Envia les capçaleres HTTP.
 */
function sendHttpHeaders() {
	header('Content-Type: text/html; charset=utf-8');
}

/**
 * Envia les capçaleres HTTP i l'inici del body.
 * @see http://runroomclients.com/fje/prototype/v3/style-guide.html
 */
function sendHtmlHeaders() {
?>
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<title><?php echo htmlentities($GLOBALS['htmlHeadTitle'], ENT_QUOTES, "UTF-8") ?>
</title>

<?php
// includeJfeStyle();
?>

</head>
<?php
}

/**
 * Inclou l'estil de FJE
 * @see http://runroomclients.com/fje/prototype/v3/style-guide.html
 */
function includeJfeStyle() {
?>
<script type="text/javascript">
<!--//--><![CDATA[//><!--
var LANET_BASE_URL = 'http://srv.fje.cat/net2/current/';
$('body').prepend('<div class="header"><div/>');
$('.header').load(LANET_BASE_URL + 'views/user_info.html', function() {
	$('.login').remove();
	$('.page-header img').each(function(index, element) {
		var img_src = ($(element).attr('src'));
		$(element).attr('src', LANET_BASE_URL + img_src);
	});
});
//--><!]]>
</script>
<?php
}

function showInterestsHeader() {
	echo Interest::getHtmlInterestsRowHeader() . "\n";
}

function showInterestsFoundPresentation() {
?>
<p>Respostes d'altres usuaris interessats en el seu llibre:</p>
<?php
}

function showInterestsFooter() {
	echo Interest::getHtmlInterestsRowFooter() . "\n";
}

function showInterests($theAd, $user) {
	foreach ($theAd->getInterests() as $anInterest) {
		echo $anInterest->toHtmlRow() . "\n";
	}
}


function showNoAdsFound() {
?>
<p>No s'han trobat llibres</p>
<?php
}

function showAdsFoundPresentation() {
?>
<p>Llibres trobats:</p>
<?php
}

function showMyAdsPresentation() {
?>
<p>Meus llibres anunciats:</p>
<?php
}

function showAdsHeaderDEPRECATED() {
	echo Ad::getHtmlAdsRowHeaderDEPRECATED() . "\n";
}
function showAdsFooter() {
	echo Ad::getHtmlAdsRowFooter() . "\n";
}

function showAdsHeaderOwn() {
	echo Ad::getHtmlAdsRowHeaderOwn() . "\n";
}

function showAdsHeaderNotOwn() {
	echo Ad::getHtmlAdsRowHeaderNotOwn() . "\n";
}

function showAdsDEPRECATED($adsArray, $user) {
	foreach ($adsArray as $anAd) {
		if($anAd->getStatus() != Ad::$STATUS_PUBLISHED) {
			echo "<!-- deleted ad -->";
			continue;
		}
		if($anAd->getOwner() == $user->getId()) {
			// $aLink = "?actionId=" . ACTION_SHOW_OTHERS_ADS . "&id=" . $anAd->getId();
			echo $anAd->toHtmlAnchoredRowDEPRECATED(Ad::$ROW_TO_DELETE) . "\n";
		}
		else {
			// $aLink = "?actionId=" . ACTION_SHOW_OTHERS_ADS . "&id=" . $anAd->getId();
			echo $anAd->toHtmlAnchoredRowDEPRECATED(Ad::$ROW_TO_BE_INTERESTED) . "\n";
		}
	}
}


function showAdsOwn($dao, $adsArray, $user) {
	foreach ($adsArray as $anAd) {
		if($anAd->getStatus() != Ad::$STATUS_PUBLISHED) {
			echo "<!-- deleted ad -->";
			continue;
		}
		if($anAd->getOwner() != $user->getId()) {
			trigger_error("Error: Recuperats anuncis d'altri quan tractant de mostrar els propis", E_USER_WARNING);
			return FALSE;
		}
		$aGrade = $dao->getGradeById($anAd->getGrade());
		echo $anAd->toHtmlAnchoredRowOwn($aGrade) . "\n";
	}
}


function showAdsNotOwn($dao, $adsArray, $user) {
	foreach ($adsArray as $anAd) {
		if($anAd->getStatus() != Ad::$STATUS_PUBLISHED) {
			echo "<!-- skip deleted ad -->";
			continue;
		}
		if($anAd->getOwner() == $user->getId()) {
			echo "<!-- skip own ad -->";
			continue;
		}
		$aGrade = $dao->getGradeById($anAd->getGrade());
		echo $anAd->toHtmlAnchoredRowNotOwn($aGrade) . "\n";
	}
}

function fillAdInterests($dao, $anAd, $user) {
	$anAd->setInterests($dao->getInterestsByAdId($anAd->getId()));	
}


function showInterestForm($ad) {
?>
<table border="1"><tr><td bgcolor="#F5F5F5">
	<p>Si tens inter&egrave;s en el llibre
		de ISBN <b><?php echo $ad->getIsbn() ?></b>
		i de resum &quot;<i><?php echo $ad->getSummary()?></i>&quot;
		deixa aqu&iacute; les teves dades de contacte i qui l'hagi anunciat
		les utilitzar&agrave; per posar-se en contacte amb tu.</p>

	<form action="?" method="post" accept-charset="utf-8">
		<input type="hidden" name="actionId"
		value="<?php echo ACTION_BE_INTERESTED_IN_ADD_DO ?>" />
		<input type="hidden" name="id" value="<?php echo $ad->getId(); ?>" />
	    <textarea name="description" rows="5" cols="50" maxlength="249"></textarea><br/>
		<input type="submit" name="Envia" value="Envia" />
	</form>		
<?php
}

/***
 * Retorna un string amb el Json que espera l'API de FJE per enviar mails.
 */
function getFjeApiJsonHtmlMsg($user, $theAd, $theAdUser, $fDescription) {
	$isbn=$theAd->getIsbn();
	$summary=$theAd->getSummary();
	$body = <<<EOBODY
<html><body><H1>Socialització de llibres</H1><p>Benvolgut/da:</p><p>Alg&uacute; est&agrave; interessat en el llibre que vas anunciar amb ISBN <b>$isbn</b> i de t&iacute;tol <b><i>$summary</i></b> i t'ha enviat aquest missatge:</p><p><b>$fDescription</b></p><p>Pots posar-te en contacte amb ell/ella. Tan de bo que pugui aprofitar el teu llibre</p><p>Moltes gr&agrave;cies per utilitzar aquest servei de socialitzaci&oacute;.</p><p>AMPA de Joan XXIII</p></body></html>
EOBODY;
	
	$to=$theAdUser->getMail();
	$fromName=$GLOBALS['MAIL_FROM_NAME'];

	$subject = scapeBody($GLOBALS['MAIL_SUBJECT']);
	$body    = scapeBody($body);
	
	$msg = <<<EOJSON
{"to":"$to","nomBustia":"$fromName","subject":"$subject","cos":"$body"}
EOJSON;

	return $msg;
}

function scapeBody($msg) {
	$msg = str_replace('\\',   '',       $msg);
	$msg = str_replace('"',    '&quot;', $msg);
	$msg = str_replace('\r\n', '<br/>',  $msg);
	$msg = str_replace('\n\r', '<br/>',  $msg);
	$msg = str_replace('\n',   '<br/>',  $msg);
	return $msg;
}

/**
 * Envia un mail a l'anunciant dient-li que algú està interessat en l'anunci. 
 * @param User $user Qui està interessat
 * @param Ad $theAd L'anunci
 * @param AD $theAdUser L'anunciant
 * @param String $fDescription El text a enviar
 * @return boolean True si OK, False si errors.
 */
function sendMailAboutInterest($user, $theAd, $theAdUser, $fDescription) {
	if($GLOBALS['MAIL_LIB'] == "PHPMailer") {
		$mail = new PHPMailer;
		$mail->CharSet = "UTF-8";
		$mail->isSMTP();										// Set mailer to use SMTP
		//Enable SMTP debugging
		// 0 = off (for production use)
		// 1 = client messages
		// 2 = client and server messages
		$mail->SMTPDebug = $GLOBALS['MAIL_DEBUG'];
		//Ask for HTML-friendly debug output
		$mail->Debugoutput = 'html';
		$mail->Host = $GLOBALS['MAIL_HOST'] ;                   // Specify main and backup SMTP servers
		// use
		// $mail->Host = gethostbyname('smtp.gmail.com');
		// if your network does not support SMTP over IPv6
		//Set the SMTP port number - 587 for authenticated TLS, a.k.a. RFC4409 SMTP submission
		$mail->Port 		= $GLOBALS['MAIL_PORT'];
		$mail->SMTPAuth 	= $GLOBALS['MAIL_SMTPAUTH'];        // Enable SMTP authentication
		$mail->Username	 	= $GLOBALS['MAIL_USERNAME'];            // SMTP username
		$mail->Password 	= $GLOBALS['MAIL_PASSWORD'];            // SMTP password
		$mail->SMTPSecure 	= $GLOBALS['MAIL_SMTPSECURE'];		// Enable encryption, 'ssl' also accepted
		
		// Nota si el compte a utilitzar és de Google: https://support.google.com/accounts/answer/6010255?hl=es
		
		$mail->From 	= $GLOBALS['MAIL_FROM'];
		$mail->FromName = $GLOBALS['MAIL_FROM_NAME'];
		$mail->addAddress($theAdUser->getMail(), $theAdUser->getFullname());	// Add a recipient
		$mail->addReplyTo($GLOBALS['MAIL_REPLYTO'], $GLOBALS['MAIL_REPLYTO_NAME']);
		//	$mail->addCC('cc@example.com');
		//	$mail->addBCC('bcc@example.com');
		
		$mail->WordWrap = 50;								// Set word wrap to 50 characters
		//	$mail->addAttachment('/var/tmp/file.tar.gz');		// Add attachments
		//	$mail->addAttachment('/tmp/image.jpg', 'new.jpg');	// Optional name
		$mail->isHTML(true);								// Set email format to HTML
		
		$theBody = "<p>Benvolgut/da:</p>"
				. "<p>Alg&uacute; est&agrave; interessat en el llibre que vas anunciar"
				. " amb ISBN <b>" . $theAd->getIsbn() . "</b> i de t&iacute;tol "
				. "<b><i>" . $theAd->getSummary() . "</i></b>"
				. " i t'ha enviat aquest missatge:</p>"
				. "<p><b>" . $fDescription . "</b></p>"
				. "<p>Pots posar-te en contacte amb ell/ella, tan de bo que pugui aprofitar el teu llibre</p>"
				. "<p>Moltes gr&agrave;cies per utilitzar aquest servei de socialització.</p>"
				. "<p>AMPA de Joan XXIII</p>";
		
		$mail->Subject = $GLOBALS['MAIL_SUBJECT'];
		$mail->Body    = $theBody;
		// $mail->AltBody = 'Una prova';
		
		// //Read an HTML message body from an external file, convert referenced images to embedded,
		// //convert HTML into a basic plain-text alternative body
		// $mail->msgHTML(file_get_contents('contents.html'), dirname(__FILE__));
		
		if(!$mail->send()) {
			trigger_error("No ha estat possible enviar el correu: " . $mail->ErrorInfo, E_USER_WARNING);
			return FALSE;
		}

	}
	elseif($GLOBALS['MAIL_LIB'] == "FJEAPI") {
		/**
		 * Utilitzant sockets, permetem posar 'allow_url_fopen' a 'false' al php.ini
		 */

		$result = TRUE;
		$resultText="Errors using mail API";

		// $GLOBALS['MAIL_FJEAPI_HTTP_HOST']='srv.fje.edu';
		// $GLOBALS['MAIL_FJEAPI_HTTP_PORT']='80';
		// $GLOBALS['MAIL_FJEAPI_HTTP_PATH']='/api/conNETctor.php/enviaInformacio';
		
		$jsonMsg=getFjeApiJsonHtmlMsg($user, $theAd, $theAdUser, $fDescription);
	
		$apiResponse='';
		$apiResponseHeaders='';
		$apiResponseBody='';
		// $host  $port  $fullpath
		$host=$GLOBALS['MAIL_FJEAPI_HTTP_HOST'];
		$port=$GLOBALS['MAIL_FJEAPI_HTTP_PORT'];
		$path=$GLOBALS['MAIL_FJEAPI_HTTP_PATH'];
// print "DEBUG: MAIL: host=$host port=$port path=$path<br/>\n";
		
		$fp = fsockopen($host, $port, $errno, $errstr, $GLOBALS['SOCKET_CONN_TIMEOUT']);
		if(function_exists('stream_set_timeout'))
			stream_set_timeout($fp, $GLOBALS['SOCKET_RW_TIMEOUT']);
		if(! $fp) {
			$result = FALSE;
			$resultText="Can't connect to mail API";
		}
		else {
			$out  = "POST $path HTTP/1.1\r\n";
			$out .= "Host: $host:$port\r\n";
			$out .= "Content-Type: application/json; charset=utf-8\r\n";
			$out .= "Content-Length: " . strlen($jsonMsg) . "\r\n";
			$out .= "Connection: Close\r\n\r\n";
			$out .= "$jsonMsg\r\n";
			
print "DEBUG: MAIL: Anem a escriure això:\n$out\n<br/>\n";
				
			fwrite($fp, $out);
			while (!feof($fp)) {
				// fgets($fp, 128);
				$apiResponse .= fread($fp, $GLOBALS['MAIL_FJEAPI_MAX_BUFF']);
				if (strlen($apiResponse) > $GLOBALS['MAIL_FJEAPI_MAX_RESPONSE_SIZE']) {
					$result = FALSE;
					$resultText="Mail API response exceeds max size (" . $GLOBALS['MAIL_FJEAPI_MAX_RESPONSE_SIZE'] . " bytes" ;
					break;
				}
			}
			fclose($fp);
print "DEBUG: API response complet: $apiResponse<br/>\n";

			/*
			 * La respost de l'API de JFE no sembla JSON compliant
			 * doncs escapa les cometes i el nom de variable acaba en ":"
			 *
			 * '"{ \"resultat:\":\"1\"}"'
			 */

// $apiResponseSmpl = preg_replace('/^.*\r\n\r\n(.*)$/', '\1', $apiResponse);
			list($apiResponseHeaders, $apiResponseBody) = explode("\r\n\r\n", $apiResponse, 2);
print "DEBUG: API response body: [$apiResponseBody]<br/>\n";
			
			/*
			$apiResponseSmpl = str_replace('\r\n', '', $apiResponseSmpl);
			$apiResponseSmpl = str_replace('\n\r', '', $apiResponseSmpl);
			$apiResponseSmpl = str_replace('\r',   '', $apiResponseSmpl);
			$apiResponseSmpl = str_replace('\n',   '', $apiResponseSmpl);
			$apiResponseSmpl = str_replace('\\',   '', $apiResponseSmpl);
			$apiResponseSmpl = str_replace('"',    '', $apiResponseSmpl);
			$apiResponseSmpl = str_replace(' ',    '', $apiResponseSmpl);			
			print "DEBUG: API response body sesgat: [$apiResponseBody]<br/>\n";
			*/

/*
aquesta regexp no funicona bé, millor passem a fer primer quelcom com:
str_replace("\r\n", '', $str);
str_replace("\n\r", '', $str);
str_replace("\n", '', $str);
str_replace("\r", '', $str);
i després simplificar la regexp treient els [\s\r\n]*
*/

			// if ($apiResponseSmpl != "{resultat::1}")
			if (preg_match("/^[\s\r\n]*\{[\s\r\n]*\"?resultat[\s\r\n]*:?[\s\r\n]*\"?[\s\r\n]*:?[\s\r\n]*\"?[\s\r\n]*1[\s\r\n]*\"?[\s\r\n]*\}[\s\r\n]*/i", $apiResponseBody)) {
				trigger_error("L'API per enviar correus sembla retornar errors: $apiResponse : $apiResponseBody" , E_USER_WARNING);
				return FALSE;
			}
			else {
				print "<b>ben enviat</b></br>\n";
				return TRUE;
			}

		}
		
		if ($result === FALSE) {
			trigger_error("Errors enviant el correu: $resultText" , E_USER_WARNING);
			return FALSE;
		}
	}
	else {
		trigger_error("Error de configuració (MAIL_LIB). No ha estat possible enviar el correu", E_USER_WARNING);
		return FALSE;
	}

	return FALSE;
}

?>
