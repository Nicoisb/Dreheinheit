function ready() {

    const knob1 = demoKnob('servospeed1');
    const knob2 = demoKnob('servospeed2');
    const knob3 = demoKnob('servospeed3');
    const knob4 = demoKnob('servospeed4');
    knob1.setValue('%SERVO1_SPEED%');
    knob2.setValue('%SERVO2_SPEED%');
    knob3.setValue('%SERVO3_SPEED%');
    knob4.setValue('%SERVO4_SPEED%');
    // const checkbox1 = setCheckbox('randompatternservo1','%SERVO1_RFLAG%');
    minslider(1);
    maxslider(1);

    minslider(2);
    maxslider(2);

    minslider(3);
    maxslider(3);

    minslider(4);
    maxslider(4);
    
}




document.addEventListener('DOMContentLoaded', ready, false);