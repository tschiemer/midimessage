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

while (!feof($fi)) {
    array_push($lines, fgets($fi));

    // keep max of 11 lines to process (number of lines used in regex)
    if (count($lines) > 11){
        array_shift($lines);
    }

    $str = join("",$lines);

    if (preg_match($regex, $str, $matches )){
        //$matches = $matches[0];


        $value = $matches[1];
        $url = $matches[3];
        $name = $matches[4];
        $status = $matches[5];

        $var = preg_replace('([^a-zA-Z]+)','', $name);

        if (preg_match('/^(?:(\d{2})H){1} (?:(\d{2})H){1} (?:(\d{2})H){1}|(?:(\d{2})H){1}$/', $value, $hex)){

            if (isset($hex[4])){
                $code = "000000{$hex[4]}";
            } else {
                $code = "00{$hex[1]}{$hex[2]}{$hex[3]}";
            }

            $str = "\t ManufacturerId{$var} \t = 0x{$code} /* ${status} / ${name} {$url} */";

            //echo "{$str} \n";

            //var_dump($hex);

            array_push( $enum, $str );
        }
    }
}

fclose( $fi );

$enum = join(",\n",$enum);

$date = date('c');

$template = file_get_contents( $templateInFile );

$outData = str_replace( ['${date}', '${manufacturerIdList}'], [$date, $enum], $template);

file_put_contents( $outFile, $outData );
