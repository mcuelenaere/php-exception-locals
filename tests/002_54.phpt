--TEST--
Check all defined exception classes for locals variable
--SKIPIF--
<?php if(PHP_VERSION_ID >= 50500) die('Skipped: requires PHP < 5.5');
--FILE--
<?php
function run() {
  foreach (get_declared_classes() as $class) {
    if (get_parent_class($class) === 'Exception') {
      echo $class . "\n";
      if ($class === 'SoapFault') {
        $obj = new $class('1', 'Error message!');
      } else {
        $obj = new $class('foo');
      }
      var_dump($obj->getTrace());
      unset($obj);
    }
  }
}

run();
--EXPECTF--
ErrorException
array(1) {
  [0]=>
  array(5) {
    ["file"]=>
    string(%d) "%s/tests/002_54.php"
    ["line"]=>
    int(17)
    ["function"]=>
    string(3) "run"
    ["args"]=>
    array(0) {
    }
    ["locals"]=>
    array(1) {
      ["class"]=>
      string(14) "ErrorException"
    }
  }
}
DOMException
array(1) {
  [0]=>
  array(5) {
    ["file"]=>
    string(%d) "%s/tests/002_54.php"
    ["line"]=>
    int(17)
    ["function"]=>
    string(3) "run"
    ["args"]=>
    array(0) {
    }
    ["locals"]=>
    array(1) {
      ["class"]=>
      string(12) "DOMException"
    }
  }
}
LogicException
array(1) {
  [0]=>
  array(5) {
    ["file"]=>
    string(%d) "%s/tests/002_54.php"
    ["line"]=>
    int(17)
    ["function"]=>
    string(3) "run"
    ["args"]=>
    array(0) {
    }
    ["locals"]=>
    array(1) {
      ["class"]=>
      string(14) "LogicException"
    }
  }
}
RuntimeException
array(1) {
  [0]=>
  array(5) {
    ["file"]=>
    string(%d) "%s/tests/002_54.php"
    ["line"]=>
    int(17)
    ["function"]=>
    string(3) "run"
    ["args"]=>
    array(0) {
    }
    ["locals"]=>
    array(1) {
      ["class"]=>
      string(16) "RuntimeException"
    }
  }
}
PharException
array(1) {
  [0]=>
  array(5) {
    ["file"]=>
    string(%d) "%s/tests/002_54.php"
    ["line"]=>
    int(17)
    ["function"]=>
    string(3) "run"
    ["args"]=>
    array(0) {
    }
    ["locals"]=>
    array(1) {
      ["class"]=>
      string(13) "PharException"
    }
  }
}
ReflectionException
array(1) {
  [0]=>
  array(5) {
    ["file"]=>
    string(%d) "%s/tests/002_54.php"
    ["line"]=>
    int(17)
    ["function"]=>
    string(3) "run"
    ["args"]=>
    array(0) {
    }
    ["locals"]=>
    array(1) {
      ["class"]=>
      string(19) "ReflectionException"
    }
  }
}
SoapFault
array(1) {
  [0]=>
  array(5) {
    ["file"]=>
    string(%d) "%s/tests/002_54.php"
    ["line"]=>
    int(17)
    ["function"]=>
    string(3) "run"
    ["args"]=>
    array(0) {
    }
    ["locals"]=>
    array(1) {
      ["class"]=>
      string(9) "SoapFault"
    }
  }
}