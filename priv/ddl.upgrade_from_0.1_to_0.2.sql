ALTER TABLE `ads` ADD `notified` BOOLEAN NOT NULL DEFAULT FALSE AFTER `grade`, ADD INDEX `idx_notified` (`notified`);
