ALTER  TABLE `liberdb`.`ads` ADD `notified` BOOLEAN NOT NULL DEFAULT FALSE AFTER `grade`, ADD INDEX `idx_notified` (`notified`);
CREATE TABLE `liberdb`.`notifyme` ( `id` INT NOT NULL AUTO_INCREMENT , `owner` VARCHAR(255) NOT NULL , `grade` INT NOT NULL , PRIMARY KEY (`id`)) ENGINE = InnoDB;
ALTER  TABLE `liberdb`.`notifyme` ADD UNIQUE KEY `uk_notifyme` (`owner`, `grade`);

