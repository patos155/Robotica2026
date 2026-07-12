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

        // getters publicos para que otros modulos puedan leer las distancias de manera segura 
        long getLRM() const {return _lrm;}
        long getLFM() const {return _lfm;}
        long getFLM() const {return _flm;}
        long getFRM() const {return _frm;}
        long getRFM() const {return _rfm;}
        long getRRM() const {return _rrm;}

        // getters privados para valores logicos (0, 1)
        bool isLRL() const {return _lrl;}
        bool isLFL() const {return _lfl;}
        bool isFLL() const {return _fll;}
        bool isFRL() const {return _frl;}
        bool isRFL() const {return _rfl;}
        bool isRRL() const {return _rrl;}

    private:
        long _lrm,_lfm,_flm,_frm,_rfm,_rrm; // Mediciones de sensores ultrasonicos en centimetros 
        bool _lrl,_lfl,_fll,_frl,_rfl,_rrl; // Valores logicos de lectura de sensores ultrasonicos

        long _readUltra(pinTrig, pinEcho);
};