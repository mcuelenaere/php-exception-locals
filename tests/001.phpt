--TEST--
Simple test
--FILE--
<?php
function d($a = 1) {
	$foo = 'bar';

	throw new Exception('foo');
}

function c() {
	$bar = array('foo');
	static $wut = 'blat';
	d();
}

class A {
	public function b() {
		$start = 'go';
		c();
	}
}

$a = 1;
$b = '2';

try {
	(new A())->b();
} catch (Exception $ex) {
	var_dump($ex->getTrace());
}
--EXPECTF--
array(3) {
  [0]=>
  array(5) {
    ["file"]=>
    string(62) "%s/tests/001.php"
    ["line"]=>
    int(11)
    ["function"]=>
    string(1) "d"
    ["args"]=>
    array(0) {
    }
    ["locals"]=>
    array(1) {
      ["foo"]=>
      string(3) "bar"
    }
  }
  [1]=>
  array(5) {
    ["file"]=>
    string(62) "%s/tests/001.php"
    ["line"]=>
    int(17)
    ["function"]=>
    string(1) "c"
    ["args"]=>
    array(0) {
    }
    ["locals"]=>
    array(2) {
      ["bar"]=>
      array(1) {
        [0]=>
        string(3) "foo"
      }
      ["wut"]=>
      &string(4) "blat"
    }
  }
  [2]=>
  array(7) {
    ["file"]=>
    string(62) "%s/tests/001.php"
    ["line"]=>
    int(25)
    ["function"]=>
    string(1) "b"
    ["class"]=>
    string(1) "A"
    ["type"]=>
    string(2) "->"
    ["args"]=>
    array(0) {
    }
    ["locals"]=>
    array(1) {
      ["start"]=>
      string(2) "go"
    }
  }
}