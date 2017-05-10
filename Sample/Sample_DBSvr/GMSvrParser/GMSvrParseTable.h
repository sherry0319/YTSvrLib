
// 有服务器连接到数据缓存服务器
GMSVRPARSER_PROC_TABLE_CLIENT( 0x0001, OnReqSvrLogin );	

// 收到来自逻辑服务器的存储请求
GMSVRPARSER_PROC_TABLE_CLIENT( 0x0002, OnReqExecSQL );	

// 查询当前数据缓存服务器队列长度
GMSVRPARSER_PROC_TABLE_CLIENT( DBMSG_QUERY, OnReqQuery );
