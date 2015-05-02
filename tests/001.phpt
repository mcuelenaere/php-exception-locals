--TEST--
Simple test
--FILE--
<?php
function d($a = 1) {
	$foo = 'bar';

	throw new RuntimeException('foo');
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
	var_dump($ex->getLocals());
}
--EXPECT--
array(4) {
  [0]=>
  array(1) {
    ["foo"]=>
    string(3) "bar"
  }
  [1]=>
  array(2) {
    ["bar"]=>
    array(1) {
      [0]=>
      string(3) "foo"
    }
    ["wut"]=>
    &string(4) "blat"
  }
  [2]=>
  array(1) {
    ["start"]=>
    string(2) "go"
  }
  [3]=>
  array(2) {
    ["a"]=>
    int(1)
    ["b"]=>
    string(1) "2"
  }
}
