<?

if(!empty($_POST['data'])){
	$data = $_POST['data'];
	$fname = "content.json";//generates random name

	$file = fopen("./" .$fname, 'w');//creates new file
	fwrite($file, $data);
	fclose($file);
}

?>