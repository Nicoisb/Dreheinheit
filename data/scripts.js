//Drehregler Lib




function demoKnob(knobId) {
    // Create knob element, 300 x 300 px in size.
    const knob = pureknob.createKnob(140, 140, knobId);

    // Set properties.
    knob.setProperty('angleStart', -0.75 * Math.PI);
    knob.setProperty('angleEnd', 0.75 * Math.PI);
    knob.setProperty('colorFG', '#00B4D8');
    knob.setProperty('trackWidth', 0.4);
    knob.setProperty('valMin', 0);
    knob.setProperty('valMax', 100);
    knob.setProperty('textScale', 0.75);

    knob.setProperty('fnValueToString', function (value) {
        let string = value.toString();
        let n = string.length;

        /*
        * If value is just a single digit, add leading zero.
        */
        if (n < 2) {
            string = '0' + string;
            n += 1;
        }

        const prefix = string.slice(0, n - 1);
        const suffix = string.slice(n - 1, n);
        const result = prefix + suffix + ' %';
        return result;
    });

    knob.setProperty('fnStringToValue', function (string) {
        let val = 0;
        const numerals = string.match(/\d*(\.\d*)?/);

        /*
         * Ensure that numerals are non-null.
         */
        if (numerals !== null) {

            /*
             * Check if we found a numeral.
             */
            if (numerals.length > 0) {
                const numeral = numerals[0];
                const f = parseFloat(numeral);
                val = Math.round(10.0 * f);
            }

        }

        return val;
    });

    // Set initial value.
    knob.setValue(50);

    const listener = function (knob, value) {
        console.log(knob, value);
        const elem = document.getElementById(knob._input.id);
        elem.setAttribute('value', value);
    };

    knob.addListener(listener);

    // Create element node.
    const node = knob.node();

    // Add it to the DOM.
    const elem = document.getElementById(knobId);
    elem.appendChild(node);

    return knob;
}


/*Slider*/
function minslider(id) {

    const elem = document.getElementById('slider-distance-' + id);
    const th = document.getElementById('minsliderservo' + id);

    th.value = Math.min(th.value, elem.childNodes[5].value - 1);
    var value = (100 / (parseInt(th.max) - parseInt(th.min))) * parseInt(th.value) - (100 / (parseInt(th.max) - parseInt(th.min))) * parseInt(th.min);
    var children = elem.childNodes[1].childNodes;
    children[1].style.width = value + '%';
    children[5].style.left = value + '%';
    children[7].style.left = value + '%'; children[11].style.left = value + '%';
    children[11].childNodes[1].innerHTML = th.value;
}

function maxslider(id) {

    const elem = document.getElementById('slider-distance-' + id);
    const th = document.getElementById('maxsliderservo' + id);

    th.value = Math.max(th.value, elem.childNodes[3].value - (-1));
    var value = (100 / (parseInt(th.max) - parseInt(th.min))) * parseInt(th.value) - (100 / (parseInt(th.max) - parseInt(th.min))) * parseInt(th.min);
    var children = elem.childNodes[1].childNodes;
    children[3].style.width = (100 - value) + '%';
    children[5].style.right = (100 - value) + '%';
    children[9].style.left = value + '%'; children[13].style.left = value + '%';
    children[13].childNodes[1].innerHTML = th.value;
}

  
