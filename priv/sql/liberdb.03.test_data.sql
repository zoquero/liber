INSERT INTO `grades` (`id`, `name`, `enabled`) VALUES (1, '1er de primària', TRUE), (2, '2on de primària', TRUE), (3, '3er de primària', TRUE), (4, '4t de primària', TRUE), (5, '5è de primària', TRUE);
INSERT INTO `liberdb`.`members` (`id`, `mail`, `fullname`) VALUES (1, 'zoquero@gmail.com', 'Angel Galindo Muñoz');
INSERT INTO `liberdb`.`members` (`id`, `mail`, `fullname`) VALUES (2, 'perico@palotes.com', 'Perico de los Palotes');
INSERT INTO `liberdb`.`ads` (`id`, `owner`, `isbn`, `status`, `summary`, `description`, `ad_publishing_date`, `image`, `grade`) VALUES (1, '1', '1234567890123', '1', 'Matemàtiques de 1er d''ESO', 'El llibre de Matemàtiques de 1er d''ESO', '2016-03-12 14:24:43', 'LiBeR_ImG_1_TESTD1', 1);
INSERT INTO `liberdb`.`ads` (`id`, `owner`, `isbn`, `status`, `summary`, `description`, `ad_publishing_date`, `image`, `grade`) VALUES (2, '1', '1234567890124', '1', 'Català de 1er d''ESO', 'El llibre de català de 1er d''ESO', '2016-03-12 14:26:54', 'LiBeR_ImG_1_TESTD2', 2);
INSERT INTO `liberdb`.`ads` (`id`, `owner`, `isbn`, `status`, `summary`, `description`, `ad_publishing_date`, `image`, `grade`) VALUES (3, '2', '1234567890125', '1', 'Ciències naturals de 2on d''ESO', 'El llibre de ciències naturals de 2on d''ESO', '2016-03-12 16:37:58', 'LiBeR_ImG_2_TESTD3', 3);
