pebble-todo
===========

A demo Pebble ToDo List app that demonstrates the ease of integration with Woozilli Cloud Platform.

For detailed tutorials please visit <https://www.woozilli.com/pebble_showcase>

#### Content Overview:

* `appinfo.json` -- meta data for the project. Note 'configurable' is present to support the configuration interface.
* `pebble-todo-config.html` -- a sample configuration interface that allows a user to add a ToDo List item when they are logged in on the Pebble mobile app. See <https://developer.getpebble.com/2/guides/javascript-guide.html> for more details. This page is compatible with Node.js & Swig.
* `src/js/pebble-js-app.js` -- platform independent PebbleKit JavaScript framework app code executed in the sandbox of the Pebble mobile app. It handles fetching/deletion of the ToDo List items stored on Woozilli Cloud Platform.
* `src/wz_pebble_todolist.c` -- Pebble watch app code that shows the ToDo List items using MenuLayer. A long press of the select button deletes the selected item(both on the watch app and on Woozilli Cloud Platform).


## License

The MIT License (MIT)

    Copyright (C) 2014 Woozilli, Inc.
    
    Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
