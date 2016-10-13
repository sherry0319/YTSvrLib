-- ----------------------------
-- Table structure for test_table
-- ----------------------------
DROP TABLE IF EXISTS `test_table`;
CREATE TABLE `test_table` (
  `id` int(11) NOT NULL,
  `string_param` varchar(256) DEFAULT NULL,
  `double_param` double(10,6) DEFAULT NULL,
  `datetime_param` datetime DEFAULT NULL,
  `bigint_param` bigint(20) DEFAULT NULL,
  `blob_param` blob,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- ----------------------------
-- Procedure structure for p_test
-- ----------------------------
DROP PROCEDURE IF EXISTS `p_test`;
DELIMITER ;;
CREATE DEFINER=`root`@`%` PROCEDURE `p_test`()
BEGIN
	SELECT * FROM test_table;
END
;;
DELIMITER ;
