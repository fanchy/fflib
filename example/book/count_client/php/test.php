
<?php

function ffcount_dbinfo($host, $port)
{
    $ch = curl_init();
    $url = "http://".$host.":".$port."/";
    curl_setopt($ch, CURLOPT_URL, $url);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
    curl_setopt($ch, CURLOPT_HEADER, 0);
    $output = curl_exec($ch);
    curl_close($ch);
    if ($output === FALSE) {
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

function ffcount_query($host, $port, $str_time, $db_name, $sql)
{
    $ch = curl_init();
    $url = "http://".$host.":".$port."/".$str_time."/".$db_name."/".rawurlencode($sql);
    curl_setopt($ch, CURLOPT_URL, $url);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
    curl_setopt($ch, CURLOPT_HEADER, 0);
    $output = curl_exec($ch);
    curl_close($ch);
    if ($output === FALSE) {
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
$db_name = "test";
$sql = "select * from dumy";
$ret = ffcount_dbinfo($host, $port);
print_r($ret);
$ret = ffcount_query($host, $port, $str_time, $db_name, $sql);
print_r($ret);
?>

