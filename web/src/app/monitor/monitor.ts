import { OnInit } from '@angular/core';
import { Chart } from 'chart.js';

export class Monitor implements OnInit {
    id: number;

    temperatureChart : Chart = null;
    humidityChart : Chart = null;
    massChart : Chart = null;

    constructor(id: number) {
        this.id = id;
    }

    ngOnInit() : void {
    }
}