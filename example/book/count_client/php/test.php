
<?php

function ffcount_query($host, $port, $str_time, $table_name, $sql)
{
    //以下为引用的内容：

    // 1. 初始化
    $ch = curl_init();
    // 2. 设置选项，包括URL
    $url = "http://".$host.":".$port."/".$str_time."/".$table_name."/".rawurlencode($sql);
    //echo $url."\n";
    curl_setopt($ch, CURLOPT_URL, $url);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
    curl_setopt($ch, CURLOPT_HEADER, 0);
    // 3. 执行并获取HTML文档内容
    $output = curl_exec($ch);
    // 4. 释放curl句柄
    curl_close($ch);
    if ($output === FALSE) {
        //echo "cURL Error: " . curl_error($ch);
        $ret = array("err_msg" =>"http request failed by curl", "col_names"=>array(), "ret_data"=>array());
    }
    else
    {
        $ret = json_decode($output);
        if (!$ret)
        {
            $ret = array("err_msg" =>$output, "col_names"=>array(), "ret_data"=>array());
        }
    }
    return $ret;
}

$host = "127.0.0.1";
$port = 8080;
$str_time = "2013/2";
$table_name = "dumy";
$sql = "select * from dumy";
$ret = ffcount_query($host, $port, $str_time, $table_name, $sql);

print_r($ret);
?>

