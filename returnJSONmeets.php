<?php
	header('Content-Type: application/json');
	header("Cache-Control: no-cache, must-revalidate");

	$directory = "./_JSON_heats/";
	$phpfiles = glob($directory . "*.json");

	$ret = array();

	foreach ($phpfiles as $key => $filename) {
		//echo $filename;
		//echo '<br>';
		//echo basename($filename, ".json");
		//echo '<br>';
		//echo '<br>';
		$ret[$key]["path"] = $filename;
		$ret[$key]["value"] = basename($filename, ".json");
	}


	//print_r($ret);
	echo json_encode($ret);
?>