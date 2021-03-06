<?php

/**
  * More info here: https://github.com/zoquero/liber
  * Licensed with GPLv3
  * Angel Galindo Muñoz
  * zoquero@gmail.com
  * 20170708
  */

define('ROOT_PATH', dirname(__FILE__));

require_once(ROOT_PATH . "/conf.inc");
// require_once(ROOT_PATH . "/class.user.inc.php"); // no user table
require_once(ROOT_PATH . "/liberlib.inc");
require_once(ROOT_PATH . "/dao.inc");
require_once(ROOT_PATH . "/class.logentry.inc.php");


set_error_handler("myErrorHandler");

// safeSessionStart(); // No gestionava bé la sessió i impossibilitava fer "back"
sendHttpHeaders();
sendHtmlHeaders();

$dao = new Dao;
if($dao->isErrConnecting()) {
	trigger_error("No ha estat possible connectar amb la base de dades", E_USER_ERROR);
}

// Capçalera i menú principal
showRegularBodyHeaders();

$user=getUser();
if($GLOBALS['debug']) {
  echo "Debug: Dades de sessió recuperades:<br>\n";
  print_r($user);
}


$GLOBALS["actionId"] = getParamInt('actionId');
if($GLOBALS['debug']) {
  showMessage("Debug: actionId = " . $GLOBALS["actionId"]);
}

doLog($dao, $GLOBALS["actionId"], $user, "Action");

switch($GLOBALS["actionId"]) {
	case ACTION_NEW_AD_FORM:
		// Cal mostrar a l'usuari el formulari per a que descrigui el seu llibre 
		$fIsbn=getSimplifiedIsbn(getParamSanitizedString('isbn'));
		$fSummary=getParamSanitizedString('summary');
		$fDescription=getParamSanitizedString('description');
		$fWhat=getParamSanitizedString('what');
		$fGrade=getParamInt('grade');
		
		if($fGrade != NULL) {
			$loadedGrade = $dao->getGradeById($fGrade);
			if($loadedGrade == NULL) {
				trigger_error("No s'ha trobat tal curs a la Base de Dades", E_USER_ERROR);
			}
		}
		else {
			$loadedGrade = NULL;
		}
		
		// hidden (arrossegant-se de previ file upload)
		$fImageBasenameH=getParamSanitizedString('imageBasenameH');
		if($GLOBALS['debug']) {
		  echo "DEBUG: fImageBasenameH=$fImageBasenameH<br>\n";
		}
		
		newAdFormShow($dao, $fIsbn, $fSummary, $fDescription, $fWhat, $fImageBasenameH, $loadedGrade);
		break;
	case ACTION_NEW_AD_PREVIEW:
		// L'usuari ha entregat les dades i ara caldrà mostrar-se-les per a confirmar-les o corregir-les
		
		/*
		if($GLOBALS['debug']) {
			echo "DEBUG: File name="     . $_FILES['imageBasenameF']['name']     ."<br/>\n";
			echo "DEBUG: File size="     . $_FILES['imageBasenameF']['size']     ."<br/>\n";
			echo "DEBUG: File type="     . $_FILES['imageBasenameF']['type']     ."<br/>\n";
			echo "DEBUG: File tmp_name=" . $_FILES['imageBasenameF']['tmp_name'] ."<br/>\n";
		}
		*/
		$fIsbn=getSimplifiedIsbn(getParamSanitizedString('isbn'));
		$fSummary=getParamSanitizedString('summary');
		$fDescription=getParamSanitizedString('description');
		$fGrade=getParamInt('grade');
		$fWhat=getParamSanitizedString('what');
		
		if($fGrade == NULL || $fGrade < 0) {
			trigger_error("Sisplau, sel·lecciona el curs al que pertany per a facilitar les cerques", E_USER_ERROR);
		}
		
		// hidden (arrossegant-se de previ file upload)
		$fImageBasenameH=getParamSanitizedString('imageBasenameH');

		$loadedGrade = $dao->getGradeById($fGrade);
		if($loadedGrade == NULL) {
			trigger_error("No s'ha trobat tal curs a la Base de Dades", E_USER_ERROR);
		}
		
		if(isset($_FILES['imageBasenameF'])
		   && file_exists($_FILES['imageBasenameF']['tmp_name'])
		   && is_uploaded_file($_FILES['imageBasenameF']['tmp_name'])) {
			$fImageBasename=compressAndSaveTmpImatge($GLOBALS['imageFilenamePrefix'] . $user['mail'] . "_");
			if($GLOBALS['debug']) {
				echo "DEBUG: fImageBasename (compressAndSaveTmpImatge) = $fImageBasename<br/>\n";
			}
		}
		else {
			$fImageBasename=$fImageBasenameH;
			if($GLOBALS['debug']) {
				echo "DEBUG: fImageBasename = $fImageBasename<br/>\n";
			}
		}

		if($fWhat == "Envia definitivament") {
			if(!createAd($dao, $user, $fIsbn, $fSummary, $fDescription, $fImageBasename, $loadedGrade)) {
				trigger_error("No s'ha pogut crear l'anunci", E_USER_ERROR);
			}
			if($fImageBasename != NULL & $fImageBasename != "") {
				rename($GLOBALS['uploadFolderTmp'].    "/" . $fImageBasename,
					   $GLOBALS['uploadFolderStore'] . "/" . $fImageBasename);
				rename($GLOBALS['uploadFolderTmp']   . "/" . $fImageBasename . $GLOBALS['thumbFilenameSufix'],
				       $GLOBALS['uploadFolderStore'] . "/" . $fImageBasename . $GLOBALS['thumbFilenameSufix']);
			}
			showMessage("S'ha donat d'alta l'anunci del llibre titolat <b><i>$fSummary</i></b>, del curs <b><i>"
						. $loadedGrade->getName(). "</i></b>, amb ISBN <b><i>$fIsbn</i></b>");

			if(sendMailConfirmNewAd($fSummary, $user['mail'])) {
				showMessage("T'hem enviat un missatge de confirmació");
			}
			else {
				trigger_error("No ha estat possible enviar-te un missatge de confirmació.", E_USER_ERROR);
			}
			
            doLog($dao, $GLOBALS["actionId"], $user, "Anunci creat de llibre amb ISBN $fIsbn");
		}
		else {
			newAdFormShow($dao, $fIsbn, $fSummary, $fDescription, $fWhat, $fImageBasename, $loadedGrade);
		}
		break;
	case ACTION_SEARCH_BY_GRADE:
		//
		$fGrade=getParamInt('grade');
		$loadedGrade = $dao->getGradeById($fGrade);
		if($loadedGrade == NULL) {
			trigger_error("No s'ha trobat tal curs a la Base de Dades", E_USER_ERROR);
		}
	
		$foundAds=$dao->getAdsByGrade($loadedGrade);
		$foundAds = removeMyAds($user, $foundAds);
		if($foundAds == NULL || sizeof($foundAds) == 0) {
			showNoAdsFound();
			doLog($dao, $GLOBALS["actionId"], $user, "Cerca per curs '" . $loadedGrade->getName() . "' (id " . $fGrade . "). 0 trobats");
		}
		else {
			showAdsFoundPresentation();
			showAdsHeaderNotOwn($user);
			showAdsNotOwn($dao, $foundAds, $user);
			showAdsFooter();
			doLog($dao, $GLOBALS["actionId"], $user, "Cerca per curs '" . $loadedGrade->getName() . "' (id " . $fGrade . "). " . sizeof($foundAds) . " trobats");
		}
		break;
	case ACTION_SEARCH_BY_ISBN:
		//
		$aIsbn=getParamSanitizedString('isbn');
		$foundAds=$dao->getAdsByIsbn($aIsbn);
		$foundAds = removeMyAds($user, $foundAds);
		if($foundAds == NULL || sizeof($foundAds) == 0) {
			showNoAdsFound();
			doLog($dao, $GLOBALS["actionId"], $user, "Cerca per ISBN '" . $aIsbn . "'. 0 trobats");
        }
        else { 
        	showAdsFoundPresentation();
			showAdsHeaderNotOwn($user);
			showAdsNotOwn($dao, $foundAds, $user);
			showAdsFooter();
			doLog($dao, $GLOBALS["actionId"], $user, "Cerca per ISBN '" . $aIsbn . "'. " . sizeof($foundAds) . " trobats");
        }
        break;
	case ACTION_SHOW_MY_ADS:
		//
		$myAds=$dao->getAdsByAdvertiser($user);
		
		if($myAds == NULL || sizeof($myAds) == 0) {
			showNoAdsFound();
		}
		else {
			foreach ($myAds as $anAd) {
				fillAdInterests($dao, $anAd, $user);
			}
			// showAdsFoundPresentation();
			showMyAdsPresentation($user);
			showAdsHeaderOwn($user);
			showAdsOwn($dao, $myAds, $user);
			showAdsFooter();
		}
		break;
	
	case ACTION_SHOW_ALL_ADS:
		// Només per a administradors
		if(! isAdmin($user)) {
			@doLog($dao, $GLOBALS["actionId"], $user, "Mostra tots els anuncis SENSE AUTORITZACIÓ");
			trigger_error("No estàs autoritzat a aquesta operació (" . $GLOBALS["actionId"] . ")", E_USER_ERROR);
		}
		else {
			$myAds=$dao->getAllAds();
			
			if($myAds == NULL || sizeof($myAds) == 0) {
				showNoAdsFound();
			}
			else {
				foreach ($myAds as $anAd) {
					fillAdInterests($dao, $anAd, $user);
				}
				showAdsPresentation();
				showAdsHeaderNotOwn($user);
				showAdsNotOwnAndMine($dao, $myAds, $user);
				showAdsFooter();
			}
			doLog($dao, $GLOBALS["actionId"], $user, "Mostra tots els anuncis");
		}
		break;
	
	case ACTION_BE_INTERESTED_IN_ADD:
		//
		$fId=getParamSanitizedString('id');
		$theAd=$dao->getAdById($fId);
		if($theAd == NULL) {
			trigger_error("No s'ha trobat tal anunci a la Base de Dades", E_USER_ERROR);
		}
		showInterestForm($theAd);
		break;
	case ACTION_DELETE_ADD:
		//
		$fId=getParamSanitizedString('id');
		if($dao->deleteAdById($fId, $user)) {
			echo "<p>L'<b>anunci ha estat esborrat</b></p>\n";
		}
		else {
			trigger_error("<p>No ha estat possible esborrar l'anunci</p>\n");
		}
		break;
	case ACTION_SEARCH_BY_KEYWORDS:
		// 
		//
		$fKeywords=getParamSanitizedString('keywords');
		$aFKeywords=explode(" ", $fKeywords);
		
		$foundAds=$dao->searchAdsByKeywords($aFKeywords);
		$foundAds = removeMyAds($user, $foundAds);
		if($foundAds == NULL || sizeof($foundAds) == 0) {
			showNoAdsFound();
			doLog($dao, $GLOBALS["actionId"], $user, "Cerca per paraules '" . $fKeywords . "'. 0 trobats");
		}
		else {
			showAdsFoundPresentation();
			showAdsHeaderNotOwn($user);
			showAdsNotOwn($dao, $foundAds, $user);
			showAdsFooter();
			doLog($dao, $GLOBALS["actionId"], $user, "Cerca per paraules '" . $fKeywords . "'. " . sizeof($foundAds) . " trobats");
		}
		break;

	case ACTION_BE_INTERESTED_IN_ADD_DO:
		//
		$fId=getParamSanitizedString('id');
		$fDescription=getParamSanitizedString('description');
		if(! containsEmail($fDescription) && ! containsTelefonNumber($fDescription)) {
			trigger_error("No s'ha trobat cap número de telefon<br/>
							ni adreça de correu electrònic al teu text.<br/>
							L'anunciant no rebrà cap altre dada<br/>
							sobre tu a part d'aquest text,<br/>
							aprofita per donar-li el teu contacte.", E_USER_ERROR);
		}
		$theAd=$dao->getAdById($fId);
		if($theAd == NULL) {
			trigger_error("No s'ha trobat tal anunci a la Base de Dades", E_USER_ERROR);
		}
		// $theAdUserMail=$dao->getUserById($theAd->getOwner()); ja no tabla d'usuaris
		$theAdUserMail=$theAd->getOwner();

		if($GLOBALS['debug']) {
			echo "Usuari id " . $user->getId() . " està interessat en anunci (id $fId) [$theAd], descripció = [$fDescription]<br/>";
		}
		if(createInterest($dao, $user, $theAd, $fDescription)) {
			showMessage("Hem enregistrat que hi estàs interessat");
			doLog($dao, $GLOBALS["actionId"], $user, "Interès enregistrat sobre anunci amb id '" . $fId . "' descr = " . $fDescription . " ...");
		}
		else {
			trigger_error("Hi ha hagut algun problema enregistrant que hi estàs interessat", E_USER_ERROR);
		}
		
		if(sendMailAboutInterest($user, $theAd, $theAdUserMail, $fDescription)) {
			showMessage("Hem enviat el teu missatge a l'anunciant i l'hem convidat a que es posi en contacte amb tu");
			doLog($dao, $GLOBALS["actionId"], $user, "Correu enviat sobre interès enregistrat associat amb anunci id '" . $fId . "'");
		}
		else {
			trigger_error("No ha estat possible enviar un missatge a l'anunciant convidant-lo a que es posi en contacte amb tu. Torna a provar-ho m&eacute;s tard.", E_USER_ERROR);
		}
		
		break;
	case ACTION_SHOW_LOGS:
		if($user == NULL || !isAdmin($user)) {
			@doLog($dao, $GLOBALS["actionId"], $user, "Mostra els logs SENSE AUTORITZACIÓ");
			trigger_error("No estàs autoritzat a fer aquesta operació (" . $GLOBALS["actionId"] . ")", E_USER_ERROR);
		}
		else {
			printLogEntries($dao);
		}
		break;
	case ACTION_RESTORE:
		//
		die("Bad link. Unimplemented");
		break;
	case ACTION_SEE_NOTIFICATIONS:
		//
		$fId=getParamSanitizedString('id');
		$theAd=$dao->getAdById($fId);
		
		if($theAd == NULL) {
			trigger_error("Error: No s'ha trobat tal anunci a la Base de Dades", E_USER_ERROR);
		}
		else {
			fillAdInterests($dao, $theAd, $user);
			showInterestsFoundPresentation();
			showInterestsHeader();
			showInterests($theAd, $user);
			showInterestsFooter();
		}
		
		break;
	case ACTION_NOTIFYME_SHOW:
		//
		showNotifyMeForm($dao, $user);
		break;
	case ACTION_NOTIFYME_DO:
		if(! $dao->clearNotifymes($user)) {
			trigger_error("Error: No s'ha pogut esborrar de la base de dades els seus interessos en ser notificat abans d'establir els nous", E_USER_ERROR);
		}
		$checkedNotifyMeGrades = array();
		if(! empty($_POST['notifymeGrades'])) {
		    if(is_array($_POST['notifymeGrades'])) {
				$checkedNotifyMeGrades = $_POST['notifymeGrades'];
		    }
		    else {
				$checkedNotifyMeGrades = array($_POST['notifymeGrades']);
		    }
		    foreach($checkedNotifyMeGrades as $notifymeGrade) {
				if(!$dao->createNotifyme($user, $notifymeGrade)) {
					trigger_error("Error: No s'ha pogut enregistrar a la base de dades el seu interès en ser notificat", E_USER_ERROR);
				}
		    }
		}
		echo "<p><b>Hem enregistrat</b> els teus interessos.
		         <br/>Les notificacions s'enviaran només <b>un cop al dia</b></p>\n";
		break;
	case ACTION_ABOUT:
		//
		showAbout();
		break;
	default:
		/* Saludem a l'usuari agafant el nom
		 * per la clau "Nom" o "nom" del hash de dades d'usuari.
		 */
		if($user != NULL && array_key_exists("Nom", $user) && $user['Nom'] != "") {
			$msg = "Hola, " . $user['Nom'] . "! Escull una opció:" ;
		}
		else if($user != NULL && array_key_exists("nom", $user) && $user['nom'] != "") {
			$msg = "<strong>Hola, " . $user['nom'] . "!</strong><br/>Escull una opció:" ;
		}
		else {
			$msg = "Escull una opció:"; 
		}
		showMessage($msg);
}
echo "<br/>\n";
showRegularOptionsMenu($dao);

// Menú d'administrador
if(isAdmin($user)) {
	echo "<br/>";
	showAdminOptionsMenu();
}
showFooter();
/*
 * No cal cridar  $dao->disconnect() doncs ja se la crida des del destructor del DAO
 */
?>
