<?php

define('ROOT_PATH', dirname(__FILE__) . "/../www");

require_once(ROOT_PATH . "/conf.inc");
// require_once(ROOT_PATH . "/class.user.inc.php"); // no user table
require_once(ROOT_PATH . "/liberlib.inc");
require_once(ROOT_PATH . "/dao.inc");
require_once(ROOT_PATH . "/class.logentry.inc.php");


set_error_handler("myCliErrorHandler");

if(in_array("-d", $argv)) {
  $debug=1;
  echo "DEBUG: Script d'enviament de correus amb notificacions:\n";
}
else {
  $debug=0;
}

$dao = new Dao;
if($dao->isErrConnecting()) {
	trigger_error("No ha estat possible connectar amb la base de dades", E_USER_ERROR);
}

// if($GLOBALS['debug']) ...

// agafem tots els anuncis encara no notificats
$ads = $dao->getUnnotifiedAds();
// agafem tots els notifyme
$notifyMes = $dao->getNotifymes();
// per cada anunci enviem un correu a cada usuari que tingui un notifyme associat a llibres del curs al que està associat el llibre
$c = 0;
foreach($ads as $anAd) {
  foreach($notifyMes as $aNotifyMe) {
    if($debug) {
      echo "DEBUG: Comparem l'anunci [$anAd] and el desig de ser notificat [$aNotifyMe]\n";
    }
  	if($anAd->getGrade() == $aNotifyMe->getGrade()) {
    	echo "Anem a enviar un mail a [" . $aNotifyMe->getOwner() . "] notificant-lo sobre anunci amb id [" . $anAd->getId() . "]\n";
  		$grade = $dao->getGradeById($aNotifyMe->getGrade());
  		if(sendMailWithNotifyMe($anAd, $aNotifyMe->getOwner(), $grade)) {
  			$c++;
  			$dummyUserForLog['mail'] = "cron";
			doLog($dao, ACTION_NOTIFYME_MAIL, $dummyUserForLog, "Correu NotifyMe enviat a " . $aNotifyMe->getOwner() . " sobre anunci " . $anAd->getId() . " del curs " . $aNotifyMe->getGrade());
		    if($debug) {
		      echo "DEBUG: Esperem " . $GLOBALS['SLEEP_SEC_BETWEEN_NOTIFICATIONS'] . " segons després d'enviar cada correu\n";
		    }
		  	sleep($GLOBALS['SLEEP_SEC_BETWEEN_NOTIFICATIONS']);
		}
		else {
			trigger_error("No ha estat possible enviar missatge NotifyMe a " . $theAdUserMail . " sobre el curs " . $aNotifyMe->getGrade(), E_USER_ERROR);
		}
  	}
  }
  /* Ara cal marcar que ja s'han enviat totes les notificacions
   * que calia de tal anunci. Així ja no ho el revisarem al proper run.
   */
  $dao->updateAdSetNotified($anAd->getId());
}
if($debug) {
  echo "DEBUG: L'script ha finalitzat. S'han enviat $c correus\n";
}
?>
