--TEST--
Test custom exception
--FILE--
<?php
class MyException extends Exception {
}

class MyRuntimeException extends RuntimeException {
}

class MyErrorException extends ErrorException {
}

function create_exception($type, $message) {
  $local_variable = 'wassup?';
  return new $type($message);
}

var_dump(create_exception('MyException', 'foobar')->getTrace());
var_dump(create_exception('MyRuntimeException', 'foobar')->getTrace());
var_dump(create_exception('MyErrorException', 'foobar')->getTrace());
--EXPECTF--
array(1) {
  [0]=>
  array(5) {
    ["file"]=>
    string(62) "%s/tests/003.php"
    ["line"]=>
    int(16)
    ["function"]=>
    string(16) "create_exception"
    ["args"]=>
    array(2) {
      [0]=>
      string(11) "MyException"
      [1]=>
      string(6) "foobar"
    }
    ["locals"]=>
    array(1) {
      ["local_variable"]=>
      string(7) "wassup?"
    }
  }
}
array(1) {
  [0]=>
  array(5) {
    ["file"]=>
    string(62) "%s/tests/003.php"
    ["line"]=>
    int(17)
    ["function"]=>
    string(16) "create_exception"
    ["args"]=>
    array(2) {
      [0]=>
      string(18) "MyRuntimeException"
      [1]=>
      string(6) "foobar"
    }
    ["locals"]=>
    array(1) {
      ["local_variable"]=>
      string(7) "wassup?"
    }
  }
}
array(1) {
  [0]=>
  array(5) {
    ["file"]=>
    string(62) "%s/tests/003.php"
    ["line"]=>
    int(18)
    ["function"]=>
    string(16) "create_exception"
    ["args"]=>
    array(2) {
      [0]=>
      string(16) "MyErrorException"
      [1]=>
      string(6) "foobar"
    }
    ["locals"]=>
    array(1) {
      ["local_variable"]=>
      string(7) "wassup?"
    }
  }
}