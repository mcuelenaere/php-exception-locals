Local variables support for PHP exceptions
==========================================

[![Latest Stable Version](http://img.shields.io/github/release/mcuelenaere/exception-locals.svg)](https://packagist.org/packages/mcuelenaere/exception-locals) [![Build Status](http://img.shields.io/travis/mcuelenaere/exception-locals.svg)](https://travis-ci.org/mcuelenaere/exception-locals)

With this PHP extension, the contents of the local variables of an exception' stacktrace are captured and exposed in the `getTrace()` method.

Installation
------------

Install from source:

```bash
	phpize && ./configure && make && sudo make install
```

Usage
-----

To get started, you only need to install this extension and activate it in your php.ini configuration.

Example
-------

This simple piece of code:

```php
	class Zoo {
		private $animals = [];

		private function generateName() {
			static $names = ['Bella', 'Max', 'Oliver'];
			return $names[array_rand($names)];
		}

		public function addAnimal($species) {
			$name = $this->generateName();

			if ($species === 'dog') {
				throw new RuntimeException("Sorry, we don't take dogs");
			}

			$this->animals[] = [
				'name' => $name,
				'species' => $species,
			];
		}
	}

	// yes, lambda's will work as well!
	$createAZoo = function () {
		$zoo = new Zoo();
		$zoo->addAnimal('cat');
		$zoo->addAnimal('bird');
		$zoo->addAnimal('dog');
		return $zoo;
	};

	try {
		$createAZoo();
	} catch (Exception $ex) {
		echo "Oh ow: " . $ex->getMessage() . "\n";
		var_export($ex->getTrace());
	}
```

will result in this output:

```
Oh ow: Sorry, we don't take dogs
array (
  0 =>
  array (
    'file' => 'php shell code',
    'line' => 29,
    'function' => 'addAnimal',
    'class' => 'Zoo',
    'type' => '->',
    'args' =>
    array (
      0 => 'dog',
    ),
    'locals' =>
    array (
      'name' => 'Oliver',
    ),
  ),
  1 =>
  array (
    'file' => 'php shell code',
    'line' => 34,
    'function' => '{closure}',
    'args' =>
    array (
    ),
    'locals' =>
    array (
      'zoo' =>
      Zoo::__set_state(array(
         'animals' =>
        array (
          0 =>
          array (
            'name' => 'Max',
            'species' => 'cat',
          ),
          1 =>
          array (
            'name' => 'Bella',
            'species' => 'bird',
          ),
        ),
      )),
    ),
  ),
)
```

which learns us that the zoo (containing 2 animals, called Max and Bella) threw an exception when we tried to add a dog called Oliver to it.