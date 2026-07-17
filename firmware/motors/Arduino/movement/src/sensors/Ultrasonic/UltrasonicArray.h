// =============================================================
// Nombre de la clase:          UltrasonicArray
// Descripcion:                 Lectura de sensores ultrasonicos
// Restricciones importantes:   único módulo que ejecuta las mediciones de sensores ultrasonicos
// =============================================================

#pragma once
#include <Arduino.h>

class UltrasonicArray {
    public:
        void begin();
        void update();

        // Valores analogicos de los sensores ultrasonicos
        long getLRM() const {return _lrm;}
        long getLFM() const {return _lfm;}
        long getFLM() const {return _flm;}
        long getFRM() const {return _frm;}
        long getRFM() const {return _rfm;}
        long getRRM() const {return _rrm;}

        // Valores logicos de los sensores ultrasonicos
        bool isLRL() const {return _lrl;}
        bool isLFL() const {return _lfl;}
        bool isFLL() const {return _fll;}
        bool isFRL() const {return _frl;}
        bool isRFL() const {return _rfl;}
        bool isRRL() const {return _rrl;}

    private:
        long _lrm,_lfm,_flm,_frm,_rfm,_rrm;
        bool _lrl,_lfl,_fll,_frl,_rfl,_rrl;

        long _readUltra(pinTrig, pinEcho);
};