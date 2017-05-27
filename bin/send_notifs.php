<?php

define('ROOT_PATH', dirname(__FILE__) . "/../www");

require_once(ROOT_PATH . "/conf.inc");
// require_once(ROOT_PATH . "/class.user.inc.php"); // no user table
require_once(ROOT_PATH . "/liberlib.inc");
require_once(ROOT_PATH . "/dao.inc");
require_once(ROOT_PATH . "/class.logentry.inc.php");


set_error_handler("myCliErrorHandler");

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
foreach($ads as $anAd) {
  foreach($notifyMes as $aNotifyMe) {
//  	echo "Un cas:\n"; 
//  	echo "  anAd = $anAd\n"; 
//  	echo "  aNotifyMe = $aNotifyMe\n"; 
  	if($anAd->getGrade() == $aNotifyMe->getGrade()) {
  		$grade = $dao->getGradeById($aNotifyMe->getGrade());
  		if(sendMailWithNotifyMe($anAd, $aNotifyMe->getOwner(), $grade)) {
			doLog($dao, ACTION_NOTIFYME_MAIL, "cron", "Correu NotifyMe enviat a " . $theAdUserMail . " sobre el curs " . $aNotifyMe->getGrade());
		}
		else {
			trigger_error("No ha estat possible enviar missatge NotifyMe a " . $theAdUserMail . " sobre el curs " . $aNotifyMe->getGrade(), E_USER_ERROR);
		}
  	}
  }
}
?>
