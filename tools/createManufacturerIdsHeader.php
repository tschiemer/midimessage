<?php

error_reporting(E_ALL);
ini_set('display_errors',1);
ini_set('error_reporting', E_ALL);
ini_set('display_startup_errors',1);

if (count($argv) != 4 ){
    echo "Usage: php {$argv[0]} <manufacturer-id-numbers.html> <manufacturerids.h.in> <manufacturerids.out>\n";
    echo "  wget https://www.midi.org/specifications-old/item/manufacturer-id-numbers \n";
    return;
}

$fname = $argv[1];
$templateInFile = $argv[2];
$outFile = $argv[3];

$regex = '/<tr>\v*<td>\v*<p>([^<>]+)<\/p>\v*<\/td>\v*<td>\v*<p>(<a href="([^"]+)".*>)?([^<>]+)(?:<\/a>)?<\/p>\v*<\/td>\v*<td>\v*<p>([^<>]+)<\/p>\v*<\/td>\v*<\/tr>/m';

$fi = fopen($fname,  "r");

$lines = [];
$enum = [];

// used to eliminate duplicates
$codes = [];

while (!feof($fi)) {
    array_push($lines, fgets($fi));

    // keep max of 11 lines to process (number of lines used in regex)
    if (count($lines) > 11){
        array_shift($lines);
    }

    $str = join("",$lines);

    if (!preg_match($regex, $str, $matches )){
    	continue;
    }
        //$matches = $matches[0];


	$obj = [
		'url' => $matches[3],
		'name' => $matches[4],
		'status' => $matches[5]
	];
	
	if ($obj['status']  == 'rescinded'){
// 		continue;
	}
		
	$code = $matches[1];

	$obj['key'] = preg_replace('([^a-zA-Z]+)','', $obj['name']);

	if (!preg_match('/^(?:(?:(?:([\da-fA-F]{2})H){1} (?:([\da-fA-F]{2})H){1} (?:([\da-fA-F]{2})H){1})|(?:([\da-fA-F]{2})H){1})$/', $code, $hex)){
		continue;
	}

	if (isset($hex[4])){
		$obj['code'] = "00{$hex[4]}0000";
	} else {
		$obj['code'] = "00{$hex[1]}{$hex[2]}{$hex[3]}";
	}
	
	// filter out duplicates
	if (array_search( $obj['code'], $codes, TRUE ) !== FALSE){
		continue;		
	}
	
	array_push( $codes, $obj['code'] );

	array_push( $enum, (object)$obj );    	    
}

fclose( $fi );

usort( $enum, function( $a, $b ) {
	$ai = intval($a->code, 16);
	$bi = intval($b->code, 16);
	if ($ai < $bi) return -1;
	if ($ai > $bi) return 1;
	return 0;
});

$enum = array_map( function( $obj ){
	return "\t\t ManufacturerId{$obj->key} \t = 0x{$obj->code} /* {$obj->status} / {$obj->name} {$obj->url} */";
}, $enum );

$enum = join(",\n",$enum);

$date = date('c');

$template = file_get_contents( $templateInFile );

$outData = str_replace( ['${date}', '${manufacturerIdList}'], [$date, $enum], $template);

file_put_contents( $outFile, $outData );
