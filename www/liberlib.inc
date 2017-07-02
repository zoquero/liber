<?php

require_once("conf.inc");

function doLog($dao, $actionId, $user, $description) {
  $dao->insertLogEntry($actionId, $user, $description);
}

function printLogEntries($dao) {
  $entries /* LogEntry */ = $dao->getLogEntries();
  showLogEntryPresentation(sizeof($entries));
  showLogEntryHeader();
  showLogEntries($entries);
  showLogEntryFooter();
}


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


function myCliErrorHandler($errno, $errstr, $errfile, $errline) {
	if (!(error_reporting() & $errno)) {
		// Codi no inclòs a l'error_reporting
		return;
	}

    switch ($errno) {
    case E_USER_ERROR:
        echo "Error: [$errno] $errstr a línia $errline de $errfile (abortem l'execució)\n";
        exit(1);
    case E_USER_WARNING:
        echo "Avís: [$errno] $errstr a línia $errline de $errfile\n";
        break;
    case E_USER_NOTICE:
        echo "Nota: [$errno] $errstr a línia $errline de $errfile\n";
        break;
    case E_NOTICE:
       echo "Nota: [$errno] $errstr a línia $errline de $errfile\n";
        break;
    default:
        echo "Error: [$errno] $errstr a línia $errline de $errfile (abortem l'execució)\n";
        exit(1);
    }
}

function echoErr() {
	echo "<table border=\"1\"><tr><td bgcolor=\"#FFC0C0\">"
			. "<b>Error:</b> [$errno] $errstr<br>"
			. "Línia $errline de $errfile<br>"
			. "</td></tr></table>";
}


/**
  Retorna el hash de valors trobats al JSON emmagatzemat el fitxer o NULL si no pot llegir o decodificar
  */
function getUserDo_FileMock() {
  $testFile = ROOT_PATH . "/" . $GLOBALS['SESS_FJEAPI_MOCKING_DATA_FILE'];
  if($GLOBALS['debug']) {
     echo "Obrirem testFile = $testFile<br>\n"; 
  }
  $myfile = fopen($testFile, "r") or die ("Unable to open the file $testFile!");
  $fileContents = fread($myfile, filesize($testFile));
  if(! $fileContents) {
    die ("Can't read from $myfile");
  }
  fclose($myfile) or die ("Unable to close the file $testFile!");
  // echo "file = $fileContents\n";
  $json = json_decode($fileContents, true);
  // print_r ($json);
  return $json;
}

/**
  * Retorna el hash de valors trobats al JSON recuperat
  * del servidor de sessions o NULL si no pot llegir o decodificar
  */
function getUserDo($token) {
  $sessUrl = $GLOBALS['SESS_FJEAPI_BASE_URL'] . "/" . $token;
  // echo "Reading contents from sess url $sessUrl\n";
  $sessContents = @file_get_contents($sessUrl);
  if(! $sessContents) {
    // die ("Errors accedint al servidor de sessions o sessió invàlida");
    return NULL;
  }
  // echo "sess = $sessContents\n";
  $json = json_decode($sessContents, true);
  // print_r ($json);
  return $json;
}

function getUserToken() {
  if($GLOBALS['mockToken']) {
	if($GLOBALS['debug']) {
		echo "DEBUG: Token de sessió fake: " . $GLOBALS['SESS_FJEAPI_COOKIE_NAME'] . " = [" . $GLOBALS['SESS_FJEAPI_MOCKING_COOKIE_VALUE'] . "]<br/>\n";
	}
    return $GLOBALS['SESS_FJEAPI_MOCKING_COOKIE_VALUE'];
  }
  else {
    if($GLOBALS['mockInjectSess']) {
      if($GLOBALS['debug']) {
		echo "DEBUG: Injectem aquest token de sessió (cookie): " . $GLOBALS['SESS_FJEAPI_COOKIE_NAME'] . " = [" . $GLOBALS['SESS_FJEAPI_MOCKING_COOKIE_VALUE'] . "]<br/>\n";
	  }
      setcookie($GLOBALS['SESS_FJEAPI_COOKIE_NAME'],
        $GLOBALS['SESS_FJEAPI_MOCKING_COOKIE_VALUE'],
        time() + (86400 * 30 * 365 * 10), "/"); // 10 anys
    }
    if($_COOKIE != NULL && array_key_exists($GLOBALS['SESS_FJEAPI_COOKIE_NAME'], $_COOKIE)) {
       	if($GLOBALS['debug']) {
			echo "DEBUG: Rebem el token de sessió (cookie): " . $GLOBALS['SESS_FJEAPI_COOKIE_NAME'] . " = [" . $_COOKIE[$GLOBALS['SESS_FJEAPI_COOKIE_NAME']] . "]<br/>\n";
		}
    
	    return $_COOKIE[$GLOBALS['SESS_FJEAPI_COOKIE_NAME']];
    }
   	if($GLOBALS['debug']) {
		echo "DEBUG: No s'ha rebut token de sessió (cookie " . $GLOBALS['SESS_FJEAPI_COOKIE_NAME'] . ")<br/>\n";
	}
    return NULL;
  }
}

function getUser() {
	if($GLOBALS['mockSess']) {
		$user = getUserDo_FileMock();
	}
	else {
		$token = getUserToken();
		if($GLOBALS['debug']) {
			echo "Token = $token<br/>\n";
		}
		$user = getUserDo($token);
	}
	if($user == NULL) {
	  $contacteAmpa=$GLOBALS['CONTACTE'];
	  $msg = <<<HTMLSTR
Oh, no hem pogut recuperar les teves dades de sessió.<br/>
Si us plau, torna a accedir a
<a href="http://www.fje.edu"><strong><em>La Net</em></strong></a>
i prova d'accedir altre cop a aquesta aplicació.<br/>
Si el problema persisteix pots contactar amb<br/>
l'$contacteAmpa per depurar-ho.
HTMLSTR;

		trigger_error($msg, E_USER_ERROR);
	}
	if(! array_key_exists("mail", $user) || $user['mail'] == "") {
	  $contacteAmpa=$GLOBALS['CONTACTE'];
	  $msg = <<<HTMLSTR
Oh, no hem trobat la teva adreça de correu electrònic<br/>
a les teves dades de sessió d'usuari FJE.<br/>
Si el problema persisteix pots contactar amb<br/>
l'$contacteAmpa per depurar-ho.
HTMLSTR;

		trigger_error($msg, E_USER_ERROR);
	}
	if(in_array($user['mail'], $GLOBALS['bannedUsers'])) {
	  $contacteAmpa=$GLOBALS['CONTACTE'];
	  $msg = <<<HTMLSTR
El teu usuari té blocat l'accés a aquesta aplicació.<br/>
Pots contactar amb l'$contacteAmpa per corregir-ho.
HTMLSTR;

		trigger_error($msg, E_USER_ERROR);
	}
	
	return $user;
}

function isAdmin($user) {
	if($user == NULL || !isset($user['mail'])) {
		return false;
	}
	return (in_array($user['mail'], $GLOBALS['appAdmins']));
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
 *
 * Exemple d'strings enviats per formulari de creació d'anunci:
 *
 * abcçÀBCË.-\//{}()=`'"!$%?¿,.-;:_[]*
 * =>
 * abcçÀBCË.-  3934         ¿ .-  _
 * 
 * 1\2/3/4{5}6(7)8=9`  z10  ' z11  " z12!z13$z14%z15?z16,z17;z18:z19
 * =>
 * 1 2 3 4 5 6 7 8 9   z10 39 z11 34 z12 z13 z14 z15 z16 z17 z18 z19
 *
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
 * Empobreis un ISBN, esborrant caracters prescindibles, per a facilitar una cerca
 * @param String $originalIsbn el isbn
 * @return String
 */
function getSimplifiedIsbn($originalIsbn) {
	return preg_replace('/[:\s\._\-]/i', '', $originalIsbn);
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
 * Escriu les capçaleres del cos, amb el menú d'opcions per a l'usuari.
 */
function showRegularBodyHeaders() {
?>
<body>
	<h1>
		<?php echo htmlentities($GLOBALS['htmlBodyTitle'], ENT_QUOTES, "UTF-8") ?>
	</h1>
	
	<table>
	  <tr bgcolor="#FFFFFF">
	  	<td bgcolor="#FFFFFF">Aplicació per a la reutilització de llibres<br/>
	      <a href="<?php echo $GLOBALS['WEB_AMPA'] ?>">
	        <?php echo $GLOBALS['NOM_AMPA'] ?>
	      </a>
	    </td>
	  	<td bgcolor="#FFFFFF">
	      <a href="<?php echo $GLOBALS['WEB_AMPA'] ?>">
	  	    <img src="images/logo.png"/></a></p>
	      </a>
	    </td>
	  </tr>
	</table>
	<br/>
<?php
}

function getEnabledGradesSelect($dao, $fGrade) {
	$grades = $dao->getEnabledGrades();
	$gradesSelect="<select name=\"grade\">\n";
	$gradesSelect.="  <option value=\"-1\">Tria un curs</option>\n";
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

function getEnabledGradesNotifymeCheckbox($dao, $user) {
	$grades        = $dao->getEnabledGrades();
	$notifyMeArray = $dao->getNotifymesByOwner($user);
	$gradesChBx  = "";
	foreach ($grades as $aGrade) {
		$selectedNotifyMe = "";
		foreach($notifyMeArray as $aCheckedNotifyMe) {
			if($aCheckedNotifyMe->getGrade() == $aGrade->getId()) {
				$selectedNotifyMe = " checked";
				break;
			}
		}
		$gradesChBx.="  <input type=\"checkbox\" name=\"notifymeGrades[]\" value=\"" . $aGrade->getId() . "\" $selectedNotifyMe/>" . $aGrade->getName() . "<br/>\n";
	}
	return $gradesChBx;
}


/**
 * Escriu el menú d'opcions per a l'usuari.
 */
function showRegularOptionsMenu($dao) {
	$gradesSelect = getEnabledGradesSelect($dao, NULL);
?>
	<table>
		<tr><th>Donació de llibres</th></tr>
		<tr><td align="center">
		  <form action="?" method="post" accept-charset="utf-8">
			<input type="submit" name="Envia"
				value="Vull donar un llibre" />
			<input type="hidden" name="actionId"
				value="<?php echo ACTION_NEW_AD_FORM ?>" />
		  </form>
        </td></tr>
		<tr><td align="center">
		  <form action="?" method="post" accept-charset="utf-8">
			<input type="submit" name="Envia"
				value="Els meus llibres publicats" />
			<input type="hidden" name="actionId"
				value="<?php echo ACTION_SHOW_MY_ADS ?>" />
		  </form>
        </td></tr>
		<tr><th>Petició de llibres</th></tr>
		<tr><td align="center">
			Busca els llibres publicats per curs:<br/>			
			<form action="?" method="post" accept-charset="utf-8">
				<input type="hidden" name="actionId"
					value="<?php echo ACTION_SEARCH_BY_GRADE ?>" />
				1) <?php echo $gradesSelect ?> 2)
				<input type="submit" name="Envia"
					value="Mostra resultats" />
			</form>
        </td></tr>
		<tr><td align="center">
			Busca llibres per ISBN:		
			<form action="?" method="post" accept-charset="utf-8">
				<input type="hidden" name="actionId"
					value="<?php echo ACTION_SEARCH_BY_ISBN ?>" />
				<input type="text" name="isbn" placeholder="<Introdueix el ISBN>" />
				<input type="submit" name="Envia" value="Mostra resultats" />
			</form>			
        </td></tr>
		<tr><td align="center">
			Busca llibres per paraules clau
			<form action="?" method="post" accept-charset="utf-8">
				<input type="hidden" name="actionId"
					value="<?php echo ACTION_SEARCH_BY_KEYWORDS ?>" /> <input
					type="text" name="keywords"  placeholder="<Introdueix paraules>" />
				<input type="submit" name="Envia"
					value="Mostra resultats" />
			</form>
        </td></tr>
		<tr><td align="center">
		  <form action="?" method="post" accept-charset="utf-8">
			<input type="submit" name="Envia"
				value="Enregistra't per ser notificat sobre nous llibres publicats" />
			<input type="hidden" name="actionId"
				value="<?php echo ACTION_NOTIFYME_SHOW ?>" />
		  </form>
        </td></tr>
		<tr><th align="center">
		  <form action="?" method="post" accept-charset="utf-8">
			<input type="submit" name="Envia"
				value="Sobre Liber" />
			<input type="hidden" name="actionId"
				value="<?php echo ACTION_ABOUT ?>" />
		  </form>
        </th></tr>
	</table>
<?php
}

/**
 * Mostra info sobre l'aplicació
 */
function showAbout() {
  $correu = $GLOBALS['MAIL_AMPA'];
?>
<p>L'aplicació <strong>FamiLyber</strong> té com a finalitat la reutilització de llibres de l'escola Jesuïtes Bellvitge. Permet que pares i mares cedeixin els seus llibres en desús a d'altres famílies que ho desitjin. És una iniciativa de l'AMPA que va en la línia del <a href="https://ca.wikipedia.org/w/index.php?title=Consum_col%C2%B7laboratiu&oldid=18443245" target="_blank">consum col·laboratiu</a> i és per això que us demanem que es faci d'una manera desinteressada i totalment solidària amb d'altres famílies de l'escola.</p>
<p>Aquesta aplicació es troba dins de l'entorn informàtic de Fundació Jesuïtes Educació i n'està integrada de forma que hi has d'accedir amb les teves credencials de la NET. S'ha dissenyat tenint en compte la privacitat. No comparteix cap dada personal de qui publica llibres, simplement facilita que els interessats en llibres puguin enviar les dades de contacte que considerin als anunciants.</p>
<p>Si tens suggeriments per millorar-la, estarem encantats que ens els facis arribar per correu electrònic a <?php echo $GLOBALS['CONTACTE'] ?> .</p>
<p>Un agraïment molt especial a tots els que han col.laborat d'una manera o una altra, de forma totalment desinteressada en aquest projecte i especialment al departament de Sistemes de la informació i comunicació de la Fundació Jesuïtes Educació que ho ha fet tècnicament possible.</p>
<p>Esperem que us resulti útil i la puguem utilitzar tots!</p>
<?php
}

/**
 * Escriu el menú d'opcions per a l'usuari.
 */
function showNotifyMeForm($dao, $user) {
  $gradesChBxs = getEnabledGradesNotifymeCheckbox($dao, $user);
  
?>
  <form action="?" method="post" accept-charset="utf-8">
	<table>
		<tr><th>Notificacions sobre nous llibres disponibles:</th></tr>
		<tr><td align="center">T'enviarem un correu quan apareguin noves publicacions.<br/>Tria els cursos del teu interès. 
		</td></tr>
		<tr><td align="center">
			<table align="center"><tr><td align="left" bgcolor="#e2e2f0">
				<?php echo $gradesChBxs ?>
			</td></tr></table>
		</td></tr>
		<tr><td align="center">
			<input type="submit" name="Enviar"
				value="Enviar" />
			<input type="hidden" name="actionId"
				value="<?php echo ACTION_NOTIFYME_DO ?>" />
		</td></tr>
	</table>
  </form>
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
			<li><a href="?actionId=<?php echo ACTION_SHOW_LOGS ?>">
				Logs</a></li>
			<li><a href="dbbackup.php">
				Backup</a></li>
			<li><a href="?actionId=<?php echo ACTION_RESTORE ?>">
				Restore</a></li>
			<li><a href="?actionId=<?php echo ACTION_SHOW_ALL_ADS ?>">
				Mostra tots els anuncis</a></li>
		</ul>
	</td></tr></table>
<?php
}

/*
 * Versió antiga
 *
function showFooter() {
?>
<p>Aplicació per a la reutilització de llibres<br/>
de l'<a href="<?php echo $GLOBALS['WEB_AMPA'] ?>">AMPA
del Centre d'Estudis Joan XXIII</a><br/>
(Fundació Jesuïtes Educació Bellvitge)<br/>
<a href="<?php echo $GLOBALS['WEB_AMPA'] ?>">
<br/>
<img src="images/logo.png"/></a></p>
</body>
</html>
<?php
}
*/


function showFooter() {
?>
</body>
</html>
<?php
}


function newAdFormShow($dao, $fIsbn, $fSummary, $fDescription, $fWhat, $fImageBasename, $fGrade) {

if($GLOBALS["actionId"] == ACTION_NEW_AD_PREVIEW) {
	if($fWhat != "Torna a editar") {
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
	<table>
		<?php if(! $doReadOnly) { ?>
		<tr>
			<td colspan="2" align="center"><b>Pas 1 de 2: Entra les dades</b></td>
		</tr>
		<tr>
			<th>ISBN:</th>
			<td align="left"><input type="text" name="isbn" value="<?php echo $fIsbn ?>"
				size="13" maxlength="20" /></td>
		</tr>
		<tr>
			<th>Títol</th>
			<td align="left"><input type="text" name="summary" value="<?php echo $fSummary ?>"
				size="50" maxlength="199" /></td>
		</tr>
		<tr>
			<th>Descripció</th>
			<td align="left"><textarea name="description" rows="5" cols="50" maxlength="249"><?php echo $fDescription ?></textarea></td>
		</tr>
		<tr>
			<th>Curs</th>
			<td align="left"><?php echo $gradesSelect ?></td>
		</tr>
		<tr>
			<th>Imatge<br/>(opcional)</th>
			<td align="left">

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

			  <input type="file" name="imageBasenameF" id="imageBasenameF" accept="image/*" />
			  <p>Avís: És possible que les imatges no es carreguin des d'alguns dispositius mòbils</p>
			</td>
		</tr>
		<tr>
			<td colspan="2">&nbsp;<input type="submit" name="what"
				value="&gt;&gt;&gt; Pas 2" />
			</td>
		</tr>
		<?php } else { ?>
		<tr>
			<td colspan="2" align="center"><b>Pas 2 de 2: Verifica les dades</b></td>
		</tr>
		<tr>
			<th>ISBN:</th>
			<td align="left"><input type="text" name="isbn" value="<?php echo $fIsbn ?>"
				size="13" maxlength="20" readonly /></td>
		</tr>
		<tr>
			<th>Títol</th>
			<td align="left"><input type="text" name="summary" value="<?php echo $fSummary ?>"
				size="50" maxlength="199" readonly /></td>
		</tr>
		<tr>
			<th>Descripció</th>
			<td align="left"><textarea name="description" rows="5" cols="50" maxlength="249" readonly><?php echo $fDescription ?></textarea></td>
		</tr>
		<tr>
			<th>Curs</th>
			<td align="left"><?php echo $gradesSelect ?></td>
		</tr>
		<tr>
			<th>Imatge</th>
			<td align="left">
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
			<td colspan="2">
			  &nbsp;<strong>Verifica</strong> les dades i torna a editar o publicar finalment
			</td>
		</tr>
		<tr>
			<td colspan="2">&nbsp;<input type="submit" name="what" value="Torna a editar" />
			  Torna al pas 1
			</td>
		</tr>
		<tr>
			<td colspan="2">&nbsp;<input type="submit" name="what" value="Envia definitivament" />
			  Publica definitivament l'anunci
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

	$imageInfo = 0;
	$imageInfo = @getimagesize($origFilename);
	if($imageInfo == 0) {
		trigger_error("Imatge buida o errors mirant la seva mida. Només suportem JPEG, GIF i PNG. Sentint-ho hauràs de tornar a crear l'anunci" , E_USER_ERROR);
	}
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
<table>
<tr><td>ISBN:</td><td><input type="text" name="isbn" value="<?php echo $fIsbn ?>" size="13" maxlength="20" readonly/></td></tr>
<tr><td>Títol</td><td><input type="text" name="summary" value="<?php echo $fSummary ?>" size="50" maxlength="199" readonly/></td></tr>
<tr><td>Descripció</td><td><textarea name="description" rows="5" cols="50" maxlength="249" readonly><?php echo $fDescription ?></textarea></td></tr>
<tr><td colspan="2">&nbsp;<input type="submit" name="CorrectOrSend" value="Corregeix"/></td></tr>
<tr><td colspan="2">&nbsp;<input type="submit" name="CorrectOrSend" value="Envia"/></td></tr>
</table>
</form>

<?php
}
*/

function removeMyAds($user, $ads) {
  $r = array();
  foreach($ads as $anAd) {
    if($user['mail'] != $anAd->getOwner()) {
      array_push($r, $anAd);
    }
  }
  return $r;
}

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
 * @deprecated a 13 de juny de 2017 vaig deshabilitar la sessió
 *   doncs no la utilitzava i per algun error del codi impedia fer "back". 
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
	/*
	header("Pragma", "no-cache");
	header("Cache-Control", "private, no-store, no-cache, must-revalidate");
	header("Expires", "0");
	*/
}

/**
 * Envia les capçaleres HTML (HTML i HEAD) i l'inici del BODY.
 * @see http://runroomclients.com/fje/prototype/v3/style-guide.html
 */
function sendHtmlHeaders() {
?>
<html>
<head>

<META HTTP-EQUIV="Pragma" CONTENT="no-cache">
<META HTTP-EQUIV="Cache-Control" CONTENT="private, no-store, no-cache, must-revalidate">
<META HTTP-EQUIV="Expires" CONTENT="0">

<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title><?php echo htmlentities($GLOBALS['htmlHeadTitle'], ENT_QUOTES, "UTF-8") ?>
</title>

<style> 

html {
  text-align: center;
  font-family: Arial;
}

table,tr,td {
	border-collapse: collapse;
	border: 0px solid #aaaaaa;
	cellspacing: 1;
	cellpadding: 1;
	margin:  2px;
	padding: 2px 2px;
	vertical-align:middle;
	margin-left: auto;
    margin-right: auto;
}
form {
     display:inline;
}
a {
    text-decoration: none;
}
#Prova td,th {
  background-color: #101010;
}

tr:nth-child(even) {background-color: #eaeaea}
tr:nth-child(odd)  {background-color: #f4f4f4}
tr:hover           {background-color: #f8f8f8} 
th                 {background-color: #66b2ff} 

input[type=text],textarea {
    width: 100%;
    padding: 4px 4px;
    margin: 4px 0;
    box-sizing: border-box;
    border: 1px solid #555;
    outline: none;
    background-color: #F5F5E5;
}

input[type=text]:focus,textarea:focus {
    background-color: #F5F5B5;
}
</style>

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
<p>Respostes d'altres usuaris interessats en el teu llibre:</p>
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
<p><strong>Anuncis de llibres trobats</strong><br/>
(Aquí no trobaràs els teus llibres publicats. Si vols veure'ls ves a l'opció
"<em><a href="?actionId=<?php echo ACTION_SHOW_MY_ADS ?>">Els meus llibres publicats</a></em>"):</p>
<?php
}

function showMyAdsPresentation() {
?>
<p>Meus llibres anunciats:</p>
<?php
}

function showAdsPresentation() {
?>
<p>TOTS els llibres anunciats:</p>
<?php
}

function showAdsHeaderDEPRECATED() {
	echo Ad::getHtmlAdsRowHeaderDEPRECATED() . "\n";
}
function showAdsFooter() {
	echo Ad::getHtmlAdsRowFooter() . "\n";
}

function showAdsHeaderOwn($user) {
	echo Ad::getHtmlAdsRowHeaderOwn($user) . "\n";
}

function showAdsHeaderNotOwn($user) {
	echo Ad::getHtmlAdsRowHeaderNotOwn($user) . "\n";
}

function showAdsDEPRECATED($adsArray, $user) {
	trigger_error("Error de programació: Mètode showAdsDEPRECATED obsolet", E_USER_ERROR);
	foreach ($adsArray as $anAd) {
		if($anAd->getStatus() != Ad::$STATUS_PUBLISHED) {
			echo "<!-- deleted ad -->";
			continue;
		}
		if($anAd->getOwner() == $user['mail']) {
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
		if($anAd->getOwner() != $user['mail']) {
			trigger_error("Error: Recuperats anuncis d'altri quan tractant de mostrar els propis", E_USER_WARNING);
			return FALSE;
		}
		$aGrade = $dao->getGradeById($anAd->getGrade());
		echo $anAd->toHtmlAnchoredRowOwn($aGrade, $user) . "\n";
	}
}


function showAdsNotOwn($dao, $adsArray, $user) {
	foreach ($adsArray as $anAd) {
		if($anAd->getStatus() != Ad::$STATUS_PUBLISHED) {
			echo "<!-- skip deleted ad -->";
			continue;
		}
		if($anAd->getOwner() == $user['mail']) {
			echo "<!-- skip own ad -->";
			continue;
		}
		$aGrade = $dao->getGradeById($anAd->getGrade());
		echo $anAd->toHtmlAnchoredRowNotOwn($aGrade, $user) . "\n";
	}
}

function showAdsNotOwnAndMine($dao, $adsArray, $user) {
	foreach ($adsArray as $anAd) {
		if($anAd->getStatus() != Ad::$STATUS_PUBLISHED) {
			echo "<!-- skip deleted ad -->";
			continue;
		}
		/*
		if($anAd->getOwner() == $user['mail']) {
			echo "<!-- skip own ad -->";
			continue;
		}
		*/
		$aGrade = $dao->getGradeById($anAd->getGrade());
		echo $anAd->toHtmlAnchoredRowNotOwn($aGrade, $user) . "\n";
	}
}

function fillAdInterests($dao, $anAd, $user) {
	$anAd->setInterests($dao->getInterestsByAdId($anAd->getId()));	
}


function showInterestForm($ad) {
?>
<br/>
<table border="1"><tr><td bgcolor="#F5F5F5">
	<p>Si estàs interessat en el llibre
		de ISBN <b><?php echo $ad->getIsbn() ?></b><br/>
		i de t&iacute;tol &quot;<i><b><?php echo $ad->getSummary()?></i></b>&quot;<br/>
		deixa aqu&iacute; les teves <b>dades de contacte</b> (e-mail o telèfon)<br/>
		i qui l'hagi anunciat les utilitzar&agrave; per posar-se en contacte amb tu.</p>
	<p>Recorda que <b>NO</b> li enviarem cap dada teva<br/>
	    a part de les que escriguis a continuaci&oacute;:</p>

	<form action="?" method="post" accept-charset="utf-8">
		<input type="hidden" name="actionId"
		value="<?php echo ACTION_BE_INTERESTED_IN_ADD_DO ?>" />
		<input type="hidden" name="id" value="<?php echo $ad->getId(); ?>" />
	    <textarea name="description" rows="5" cols="50" maxlength="249"></textarea><br/>
		<input type="submit" name="Envia" value="Envia" />
	</form>
</table>
<br/>

<?php
}


function showLogEntryPresentation($numEntries) {
	echo LogEntry::showLogEntryPresentation($numEntries) . "\n";
}


function showLogEntryHeader() {
	echo LogEntry::getHtmlHeader() . "\n";
}


function showLogEntryFooter() {
	echo LogEntry::getHtmlFooter() . "\n";
}

function showLogEntries($logEntries) {
	foreach ($logEntries as $aLogEntry) {
		echo $aLogEntry->toHtmlRow() . "\n";
	}
}

/**
  * Verifica si l'string, a més d'altres coses, té quelcom que sembli una adreça de correu electrònic
  * Em conformo amb validar si té algun caracter diferent a punt i a espai abans i després de l'arroba, després un punt i algun caracter diferent a . i a espai.
  */
function containsEmail($str) {
	$regex = '/[^\\s\.]*@[^\\s\.]+\.[^\\s\.]/';
	if (preg_match($regex, $str)) {
		return true;
	}
	else {
		return false;
	}
}

/**
  * Verifica si l'string, a més d'altres coses, té quelcom que sembli un telefon
  * Em conformo amb validar si un cop empobrit, té 9 dígits consecutius
  */
function containsTelefonNumber($str) {
	$toRemove = array(" ", "-", ".", "/", "+");
	$simplifiedString = str_replace($toRemove, "", $str);
	if (preg_match("/\d{9,}/", $simplifiedString)) {
		return true;
	}
	else {
		return false;
	}
}


/***
 * Retorna un string amb el Json que espera l'API de FJE per enviar mails.
 */
function getFjeApiJsonHtmlMsg($user, $theAd, $theAdUserMail, $fDescription) {
	$isbn=$theAd->getIsbn();
	$summary=$theAd->getSummary();
	$url = $GLOBALS['LIBER_URL'];
	$titol=$GLOBALS['htmlBodyTitle'];
	$facebook=$GLOBALS['FACEBOOK_AMPA'];
	$body = <<<EOBODY
<html>
  <body>
    <H1>$titol</H1>
    <p>Benvolgut/da:</p>
    <p>Alg&uacute; est&agrave; interessat en el llibre que vas anunciar amb ISBN <b>$isbn</b>
      i de t&iacute;tol <b><i>$summary</i></b>
      i t'ha enviat aquest missatge:</p><hr/><p><b>$fDescription</b><hr/></p>
    <p>Pots posar-te en contacte amb ell/ella. Tan de bo que pugui aprofitar el teu llibre</p>
    <p>Un cop hagis cedit el llibre pots accedir a l'aplicaci&oacute;
      <a href='$url' target="_blank">'FamiLyber' $url</a> per esborrar l'anunci per evitar seguir rebent notificacions..</p>
    <p>Moltes gr&agrave;cies per utilitzar aquest servei de reutilitzaci&oacute;. Ens agradaria que compartissis la teva experiència al nostre <a href='$facebook' target="_blank">mur de facebook</a>.</p>
    
    <p>AMPA Jesu&iuml;tes Bellvitge CE Joan XXIII</p>
    </body></html>
EOBODY;
	
	$to=$theAdUserMail;
	$fromName=$GLOBALS['MAIL_FROM_NAME'];

	$subject = scapeBody($GLOBALS['MAIL_SUBJECT']);
	$body    = scapeBody($body);
	
	$msg = <<<EOJSON
{"to":"$to","nomBustia":"$fromName","subject":"$subject","cos":"$body"}
EOJSON;

	return $msg;
}

/***
 * Retorna un string amb el Json que espera l'API de FJE per enviar mails.
 */
function getFjeApiJsonHtmlMsgNotifyMe($theAd, $theAdUserMail, $grade) {
	$isbn=$theAd->getIsbn();
	$summary=$theAd->getSummary();
	$gradeName = $grade->getName();
	$url = $GLOBALS['LIBER_URL'];
	$titol=$GLOBALS['htmlBodyTitle'];
	$body = <<<EOBODY
<html>
  <body>
  <H1>$titol</H1>
  <p>Benvolgut/da:</p>
  <p>Alg&uacute; ha publicat un anunci d'un llibre
     del curs <strong>$gradeName</strong> i vas subscriure't
     a rebre notificacions quan es publiquen llibres de tal curs.</p>
  <p>Pots accedir a l'aplicaci&oacute; <a href='$url' target="_blank">'Liber' $url</a>
    per mirar els anuncis i actualitzar la teva subscripci&oacute;
    a rebre notificacions de nous llibres anunciats</p>
  <p>Moltes gr&agrave;cies per utilitzar aquest
     servei de reutilitzaci&oacute;.</p>
  <p>AMPA Jesu&iuml;tes Bellvitge CE Joan XXIII</p>
  </body></html>
EOBODY;
	
	$to=$theAdUserMail;
	$fromName=$GLOBALS['MAIL_FROM_NAME'];

	$subject = scapeBody($GLOBALS['MAIL_SUBJECT_NOTIFYME']);
	$body    = scapeBody($body);
	
	$msg = <<<EOJSON
{"to":"$to","nomBustia":"$fromName","subject":"$subject","cos":"$body"}
EOJSON;

	return $msg;
}


/***
 * Retorna un string amb el Json que espera l'API de FJE per enviar mail de confirmació.
 */
function getFjeApiJsonHtmlMsgConfirmNewAd($theAdTitle, $theAdUserMail) {
	$url = $GLOBALS['LIBER_URL'];
	$titol=$GLOBALS['htmlBodyTitle'];
	$body = <<<EOBODY
<html>
  <body>
  <H1>$titol</H1>
  <p>Benvolgut/da:</p>
  <p>Hem guardat el teu anunci de llibre "<em>$theAdTitle</em>". Et notificarem per correu si algú n'està interessat.</p>
  <p>Un cop el donis, et convidem a donar-lo de baixa a l'aplicació per evitar seguir rebent notificacions.</p>
  <p>Moltes gr&agrave;cies per utilitzar aquest servei.</p>
  <p>AMPA Jesu&iuml;tes Bellvitge CE Joan XXIII</p>
  </body></html>
EOBODY;
	
	$to=$theAdUserMail;
	$fromName=$GLOBALS['MAIL_FROM_NAME'];

	$subject = scapeBody($GLOBALS['MAIL_SUBJECT_CONFIRM']);
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
 * @param AD $theAdUserMail L'anunciant
 * @param String $fDescription El text a enviar
 * @return boolean True si OK, False si errors.
 */
function sendMailAboutInterest($user, $theAd, $theAdUserMail, $fDescription) {
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
		$mail->addAddress($theAdUserMail, $theAdUserMail);	// Add a recipient
		$mail->addReplyTo($GLOBALS['MAIL_REPLYTO'], $GLOBALS['MAIL_REPLYTO_NAME']);
		//	$mail->addCC('cc@example.com');
		//	$mail->addBCC('bcc@example.com');
		
		$mail->WordWrap = 50;								// Set word wrap to 50 characters
		//	$mail->addAttachment('/var/tmp/file.tar.gz');		// Add attachments
		//	$mail->addAttachment('/tmp/image.jpg', 'new.jpg');	// Optional name
		$mail->isHTML(true);								// Set email format to HTML
		
		$facebook=$GLOBALS['FACEBOOK_AMPA'];
		$theBody = "<p>Benvolgut/da:</p>"
				. "<p>Alg&uacute; est&agrave; interessat en el llibre que vas anunciar"
				. " amb ISBN <b>" . $theAd->getIsbn() . "</b> i de t&iacute;tol "
				. "<b><i>" . $theAd->getSummary() . "</i></b>"
				. " i t'ha enviat aquest missatge:</p>"
				. "<p><b>" . $fDescription . "</b></p>"
				. "<p>Pots posar-te en contacte amb ell/ella, tan de bo que pugui aprofitar el teu llibre</p>"
				. "<p>Moltes gr&agrave;cies per utilitzar aquest servei de reutilitzaci&oacute;. Ens agradaria que compartissis la teva experiència al nostre <a href='$facebook' target=\"_blank\">mur de facebook</a>.</p>"
				. "<p>AMPA Jesu&iuml;tes Bellvitge CE Joan XXIII</p>";
		
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
		
		$jsonMsg=getFjeApiJsonHtmlMsg($user, $theAd, $theAdUserMail, $fDescription);
	
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
			
			if($GLOBALS['debug']) {
				print "DEBUG: MAIL: Anem a escriure això:<br/>\n<pre><code>$out</code></pre><br/>\n";
			}
			
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
			
			if($GLOBALS['debug']) {
				print "DEBUG: API response complet:<br/>\n<pre><code>$apiResponse</code></pre><br/>\n";
			}


/*
 * Exemple de resposta davant d'error:
 HTTP/1.1 500 Internal Server Error
Date: Mon, 01 May 2017 20:20:40 GMT
Server: Apache/2.4.7 (Ubuntu)
X-Powered-By: PHP/5.5.9-1ubuntu4.21
Content-Length: 1466
Connection: close
Content-Type: text/html

 

 
<html><head><title>Slim Application Error</title>...
 */

/*
 * Exemple de resposta exitosa:
HTTP/1.1 200 OK
Date: Mon, 01 May 2017 20:24:29 GMT
Server: Apache/2.4.7 (Ubuntu)
X-Powered-By: PHP/5.5.9-1ubuntu4.21
Content-Length: 7
Connection: close
Content-Type: text/html

 {  "Resultat"  : "Ok"} 



 */
 
			list($apiResponseHeaders, $apiResponseBody) = explode("\r\n\r\n", $apiResponse, 2);

			$simplifiedApiResponseBody = preg_replace('/[\s"]+/S', "", $apiResponseBody);

			if($GLOBALS['debug']) {
				print "DEBUG: API response body: [$apiResponseBody]<br/>\n";
				print "DEBUG: API response body sesgat: [$simplifiedApiResponseBody]<br/>\n";
				print "DEBUG: Referència = [" . $GLOBALS['MAIL_FJEAPI_OKRESPONSE_SIMPLIFIED'] . "]<br/>\n";
			}

			$sarbLower = strtolower($simplifiedApiResponseBody);
			$refLower  = strtolower($GLOBALS['MAIL_FJEAPI_OKRESPONSE_SIMPLIFIED']);
			
			if (strcmp($sarbLower, $refLower) != 0) {
				trigger_error("L'API per enviar correus sembla retornar errors ($apiResponseBody). Probablement el correu no s'enviï, et recomanem esperar i reintentar fins que funcioni." , E_USER_WARNING);
				return FALSE;
			}
			else {
				print "<b>Correu enviat amb èxit</b></br>\n";
				return TRUE;
			}

		}
		
		if ($result === FALSE) {
			trigger_error("Errors enviant el correu: $resultText" , E_USER_WARNING);
			return FALSE;
		}
	}
	else {
		trigger_error("Error de configuració (MAIL_LIB). No ha estat possible enviar el correu. Probablement el correu no s'enviï, et recomanem esperar i reintentar fins que funcioni.", E_USER_WARNING);
		return FALSE;
	}

	return FALSE;
}


/**
 * Envia un mail a algú notificant-li que hi ha un nou llibre d'un curs del que busca llibres 
 * @param Ad $theAd L'anunci
 * @param AD $theAdUserMail L'anunciant
 * @param String $grade grade Object
 * @return boolean True si OK, False si errors.
 */
function sendMailWithNotifyMe($theAd, $theAdUserMail, $grade) {
		/**
		 * Utilitzant sockets, permetem posar 'allow_url_fopen' a 'false' al php.ini
		 */

		$result = TRUE;
		$resultText="Errors using mail API";

		// $GLOBALS['MAIL_FJEAPI_HTTP_HOST']='srv.fje.edu';
		// $GLOBALS['MAIL_FJEAPI_HTTP_PORT']='80';
		// $GLOBALS['MAIL_FJEAPI_HTTP_PATH']='/api/conNETctor.php/enviaInformacio';
		
		$jsonMsg=getFjeApiJsonHtmlMsgNotifyMe($theAd, $theAdUserMail, $grade);
	
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
			
			if($GLOBALS['debug']) {
				print "DEBUG: MAIL: Anem a escriure això:<br/>\n<pre><code>$out</code></pre><br/>\n";
			}
			
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
			
			if($GLOBALS['debug']) {
				print "DEBUG: API response complet:<br/>\n<pre><code>$apiResponse</code></pre><br/>\n";
			}


/*
 * Exemple de resposta davant d'error:
 HTTP/1.1 500 Internal Server Error
Date: Mon, 01 May 2017 20:20:40 GMT
Server: Apache/2.4.7 (Ubuntu)
X-Powered-By: PHP/5.5.9-1ubuntu4.21
Content-Length: 1466
Connection: close
Content-Type: text/html

 

 
<html><head><title>Slim Application Error</title>...
 */

/*
 * Exemple de resposta exitosa:
HTTP/1.1 200 OK
Date: Mon, 01 May 2017 20:24:29 GMT
Server: Apache/2.4.7 (Ubuntu)
X-Powered-By: PHP/5.5.9-1ubuntu4.21
Content-Length: 7
Connection: close
Content-Type: text/html

 {  "Resultat"  : "Ok"} 



 */
 
			list($apiResponseHeaders, $apiResponseBody) = explode("\r\n\r\n", $apiResponse, 2);

			$simplifiedApiResponseBody = preg_replace('/[\s"]+/S', "", $apiResponseBody);

			if($GLOBALS['debug']) {
				print "DEBUG: API response body: [$apiResponseBody]<br/>\n";
				print "DEBUG: API response body sesgat: [$simplifiedApiResponseBody]<br/>\n";
				print "DEBUG: Referència = [" . $GLOBALS['MAIL_FJEAPI_OKRESPONSE_SIMPLIFIED'] . "]<br/>\n";
			}

			$sarbLower = strtolower($simplifiedApiResponseBody);
			$refLower  = strtolower($GLOBALS['MAIL_FJEAPI_OKRESPONSE_SIMPLIFIED']);
			
			if (strcmp($sarbLower, $refLower) != 0) {
				trigger_error("L'API per enviar correus sembla retornar errors ($apiResponseBody). Probablement el correu no s'enviï, et recomanem esperar i reintentar fins que funcioni." , E_USER_WARNING);
				return FALSE;
			}
			else {
				print "<b>Correu enviat amb èxit</b></br>\n";
				return TRUE;
			}

		}
		
		if ($result === FALSE) {
			trigger_error("Errors enviant el correu: $resultText" , E_USER_WARNING);
			return FALSE;
		}
	return FALSE;
}


/**
 * Envia un mail a algú notificant-li que s'ha guardat el seu nou anunci. 
 * @param Ad $theAd L'anunci
 * @param AD $theAdUserMail L'anunciant
 * @param String $grade grade Object
 * @return boolean True si OK, False si errors.
 */
function sendMailConfirmNewAd($theAdTitle, $theAdUserMail) {
		$result = TRUE;
		$resultText="Errors using mail API";

		$jsonMsg=getFjeApiJsonHtmlMsgConfirmNewAd($theAdTitle, $theAdUserMail);
	
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
			
			if($GLOBALS['debug']) {
				print "DEBUG: MAIL: Anem a escriure això:<br/>\n<pre><code>$out</code></pre><br/>\n";
			}
			
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
			
			if($GLOBALS['debug']) {
				print "DEBUG: API response complet:<br/>\n<pre><code>$apiResponse</code></pre><br/>\n";
			}
 
			list($apiResponseHeaders, $apiResponseBody) = explode("\r\n\r\n", $apiResponse, 2);

			$simplifiedApiResponseBody = preg_replace('/[\s"]+/S', "", $apiResponseBody);

			if($GLOBALS['debug']) {
				print "DEBUG: API response body: [$apiResponseBody]<br/>\n";
				print "DEBUG: API response body sesgat: [$simplifiedApiResponseBody]<br/>\n";
				print "DEBUG: Referència = [" . $GLOBALS['MAIL_FJEAPI_OKRESPONSE_SIMPLIFIED'] . "]<br/>\n";
			}

			$sarbLower = strtolower($simplifiedApiResponseBody);
			$refLower  = strtolower($GLOBALS['MAIL_FJEAPI_OKRESPONSE_SIMPLIFIED']);
			
			if (strcmp($sarbLower, $refLower) != 0) {
				trigger_error("L'API per enviar correus sembla retornar errors ($apiResponseBody). Probablement el correu no s'enviï, et recomanem esperar i reintentar fins que funcioni." , E_USER_WARNING);
				return FALSE;
			}
			else {
				// Ok!!
				// print "<b> hem enviat un correu de confirmació a $theAdUserMail</b></br>\n";
				return TRUE;
			}

		}
		
		if ($result === FALSE) {
			trigger_error("Errors enviant el correu: $resultText" , E_USER_WARNING);
			return FALSE;
		}
	return FALSE;
}


?>
