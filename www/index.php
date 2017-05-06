<?php

define('ROOT_PATH', dirname(__FILE__));

require_once(ROOT_PATH . "/conf.inc");
// require_once(ROOT_PATH . "/class.user.inc.php"); // no user table
require_once(ROOT_PATH . "/liberlib.inc");
require_once(ROOT_PATH . "/dao.inc");

set_error_handler("myErrorHandler");

safeSessionStart();
sendHttpHeaders();
sendHtmlHeaders();

$dao = new Dao;
if($dao->isErrConnecting()) {
	trigger_error("No ha estat possible connectar amb la base de dades", E_USER_ERROR);
}

// Capçalera i menú principal
showRegularBodyHeaders();
showRegularOptionsMenu($dao);

$user=getUser();
if($GLOBALS['debug']) {
  echo "Debug: Dades de sessió recuperades:<br>\n";
  print_r($user);
}

// Menú d'administrador
if(isAdmin($user)) {
	echo "<br/>";
	showAdminOptionsMenu();
}


$GLOBALS["actionId"]=getParamInt('actionId');
// showMessage("Liber: actionId = " . $GLOBALS["actionId"]);
switch($GLOBALS["actionId"]) {
	case ACTION_SET_DUMMY_USERNAME:
		// Establir dummyUsername
		setDummyUsername();
		break;
	case ACTION_NEW_AD_FORM:
		// Cal mostrar a l'usuari el formulari per a que descrigui el seu llibre 
		$fIsbn=getParamSanitizedString('isbn');
		$fSummary=getParamSanitizedString('summary');
		$fDescription=getParamSanitizedString('description');
		$fWhat=getParamSanitizedString('what');
		$fGrade=getParamInt('grade');
//print("fGrade=$fGrade\n");
		
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
//		// nou file upload
//		$fImageBasenameF=getParamSanitizedString('imageBasenameF');
//		$fImageBasename = empty($fImageBasenameF) ? $fImageBasenameH : $fImageBasenameF;
		
		newAdFormShow($dao, $fIsbn, $fSummary, $fDescription, $fWhat, $fImageBasenameH, $loadedGrade);
		break;
	case ACTION_NEW_AD_PREVIEW:
		// L'usuari ha entregat les dades i ara caldrà mostrar-se-les per a confirmar-les o corregir-les
		
		/*
		echo "name=" . $_FILES['imageBasenameF']['name'] ."<br/>\n";
		echo "size=" . $_FILES['imageBasenameF']['size'] ."<br/>\n";
		echo "type=" . $_FILES['imageBasenameF']['type'] ."<br/>\n";
		echo "tmp_name=" . $_FILES['imageBasenameF']['tmp_name'] ."<br/>\n";
		*/
		$fIsbn=getParamSanitizedString('isbn');
		$fSummary=getParamSanitizedString('summary');
		$fDescription=getParamSanitizedString('description');
		$fGrade=getParamInt('grade');
		$fWhat=getParamSanitizedString('what');
		
		// hidden (arrossegant-se de previ file upload)
		$fImageBasenameH=getParamSanitizedString('imageBasenameH');
//		// nou file upload
//		$fImageBasenameF=getParamSanitizedString('imageBasenameF');
//		$fImageBasename = empty($fImageBasenameF) ? $fImageBasenameH : $fImageBasenameF;

		$loadedGrade = $dao->getGradeById($fGrade);
		if($loadedGrade == NULL) {
			trigger_error("No s'ha trobat tal curs a la Base de Dades", E_USER_ERROR);
		}
		
		if(isset($_FILES['imageBasenameF']) && file_exists($_FILES['imageBasenameF']['tmp_name']) && is_uploaded_file($_FILES['imageBasenameF']['tmp_name'])) {
			$fImageBasename=compressAndSaveTmpImatge($GLOBALS['imageFilenamePrefix'] . $user->getId() . "_");
		}
		else {
			$fImageBasename=$fImageBasenameH;
		}

		if($fWhat == "Envia") {
			$createdAd = createAd($dao, $user, $fIsbn, $fSummary, $fDescription, $fImageBasename, $loadedGrade);
			rename($GLOBALS['uploadFolderTmp'] . "/" . $fImageBasename, $GLOBALS['uploadFolderStore'] . "/" . $fImageBasename);
			rename($GLOBALS['uploadFolderTmp'] . "/" . $fImageBasename . $GLOBALS['thumbFilenameSufix'], $GLOBALS['uploadFolderStore'] . "/" . $fImageBasename . $GLOBALS['thumbFilenameSufix']);
			showMessage("S'ha donat d'alta l'anunci del llibre amb ISBN <b><i>$fIsbn</i></b> i sumari <b><i>$fSummary</i></b>");			
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
		if($foundAds == NULL || sizeof($foundAds) == 0) {
			showNoAdsFound();
		}
		else {
			showAdsFoundPresentation();
			showAdsHeaderNotOwn();
			showAdsNotOwn($dao, $foundAds, $user);
			showAdsFooter();
		}
		break;
	case ACTION_SEARCH_BY_ISBN:
		//
		$aIsbn=getParamSanitizedString('isbn');
		$foundAds=$dao->getAdsByIsbn($aIsbn);
		if($foundAds == NULL || sizeof($foundAds) == 0) {
			showNoAdsFound();
        }
        else { 
        	showAdsFoundPresentation();
			showAdsHeaderNotOwn();
			showAdsNotOwn($dao, $foundAds, $user);
			showAdsFooter();
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
			showMyAdsPresentation();
			showAdsHeaderOwn();
			showAdsOwn($dao, $myAds, $user);
			showAdsFooter();
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
		if($dao->deleteAdById($fId, $user->getId())) {
			echo "<p>Anunci esborrat</p>\n";
		}
		else {
			trigger_error("<p>No ha estat possible esborrar l'anunci</p>\n");
		}
		break;
/*
	case ACTION_SHOW_OTHERS_ADS:
		//
		
trigger_error("AQUESTA ACCIÓ MAI SALTARÀ, UN COP DEPURAT HO ESBORRAREM.", E_USER_ERROR);

		$fId=getParamSanitizedString('id');
		echo "<p> mirarem anunci amb id = " . $fId;
		break;
*/
	case ACTION_SEARCH_BY_KEYWORDS:
		// 
		//
		$fKeywords=getParamSanitizedString('keywords');
		$aFKeywords=explode(" ", $fKeywords);
		
		$foundAds=$dao->searchAdsByKeywords($aFKeywords);
		if($foundAds == NULL || sizeof($foundAds) == 0) {
			showNoAdsFound();
		}
		else {
			showAdsFoundPresentation();
			showAdsHeaderNotOwn();
			showAdsNotOwn($dao, $foundAds, $user);
			showAdsFooter();
		}
		break;

	case ACTION_BE_INTERESTED_IN_ADD_DO:
		//
		$fId=getParamSanitizedString('id');
		$fDescription=getParamSanitizedString('description');
		$theAd=$dao->getAdById($fId);
		if($theAd == NULL) {
			trigger_error("No s'ha trobat tal anunci a la Base de Dades", E_USER_ERROR);
		}
		// $theAdUserMail=$dao->getUserById($theAd->getOwner()); ja no tabla d'usuaris
		$theAdUserMail=$theAd->getOwner();
		// ja no cal validar-ho
		// if($theAdUserMail == NULL) {
		// 	trigger_error("No s'ha trobat l'usuari propietari de l'anunci a la Base de Dades", E_USER_ERROR);
		//}
// echo "Usuari id " . $user->getId() . " té interès en anunci (id $fId) [$theAd], descripció = [$fDescription]<br/>";
		if(createInterest($dao, $user, $theAd, $fDescription)) {
//		if(createInterest($dao, $user, $theAd, $theAdUserMail, $fDescription))
			// function createInterest($dao, $user, $theAd, $fDescription) 
			showMessage("Hem enregistrat el teu inter&egrave;s");
		}
		else {
			trigger_error("Hi ha hagut algun problema enregistrant el teu inter&egrave;s", E_USER_ERROR);
		}
		
		if(sendMailAboutInterest($user, $theAd, $theAdUserMail, $fDescription)) {
			showMessage("Hem enviat el teu missatge a l'anunciant i l'hem convidat a que es posi en contacte amb tu");
		}
		else {
			trigger_error("No ha estat possible enviar un missatge a l'anunciant convidant-lo a que es posi en contacte amb tu. Torna a provar-ho m&eacute;s tard.", E_USER_ERROR);
		}
		
		break;
	case ACTION_BACKUP:
		die("Bad link");
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
	default:
		showMessage("Escull una opció");
}

showFooter();
?>
