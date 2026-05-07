{
    "?注意?":"下列以问号(?)开始或结尾的行都为注释说明",
    "?注意?":"修改该配置文件后，如果想使得修改后的配置生效，需要将正在运行(如果程序在运行的话)的程序停止后，再重启",
    
    "?local_listening_ip?":"该http服务程序监听的ip地址",
    "local_listening_ip":"127.0.0.1",

    "?http_listening_port?":"该http服务程序的默认监听端口",
    "http_listening_port":"20011",
  
    "?model_updating_port?":"该端口为更新模型端口默认监听端口",
    "model_updating_port":"20013",

    "?algo_host_ip?":"算法主机的地址，如果该配置为空或者没有配置时，默认使用127.0.0.1作为算法主机的ip地址",
    "algo_host_ip":"10.6.220.136",

    "?algo_upt_file_path?":"算法更新文件下载本地存放路径",
    "algo_upt_file_path":"/home/sunri/algoUpt/",

    "?multi_working_process?":"当客户端请求量较大时，且为了保证客户端响应的实时性，可以将本选项置为 true，同时设置working_process_num的数量, 开启多个工作进程模式， 提高客户端响应速度,降低服务端响应时延",
    "multi_working_process":"false",

    "?single_daemon_controling_file_path?":"守护进程控制文件的生成路径。注意--该路径只能填写该用户拥有文件生成权限的路径",
    "single_daemon_controling_file_path":"/home/sunri/",
    
    "?working_process_num?":"该选项设置http服务程序工作进程数量(填入的有效值最小值为2, 最大值为5),该选项只在multi_working_process为true时，才有效 ",
    "working_process_num": 3
    
}
