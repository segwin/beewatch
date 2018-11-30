import { Component, OnInit } from '@angular/core';
import { Monitor } from './monitor';
import { Chart } from 'chart.js'

@Component({
  selector: 'app-monitor',
  templateUrl: './monitor.component.html',
  styleUrls: ['./monitor.component.css']
})

export class MonitorComponent implements OnInit {
  constructor() {}

  private async delay(ms: number) {
      await new Promise(resolve => setTimeout(()=>resolve(), ms)).then(()=>console.log("fired"));
  }

  private async initCharts() {
    // TODO: How can we do this cleanly?
    while (!document.getElementById('temperature-' + this.monitors[0].id.toString())) {
      await this.delay(50);
    }

    this.monitors.forEach((monitor) => {
      let timeMockData = [ 1542772336, 1543772336, 1544772336, 1545772336, 1546772336 ];
      let timeMockDataFmt : string[] = [];
      timeMockData.forEach((res) => { timeMockDataFmt.push((new Date(res * 1000)).toLocaleTimeString('en', { year: 'numeric', month: 'short', day: 'numeric' })) });

      let temperatureIntMockData = [ 5.0, 6.0, 3.0, 4.0, 3.0 ];
      let temperatureExtMockData = [ -6.0, -2.0, 1.0, -4.0, -11.0 ];
      let humidityIntMockData = [ 40.0, 45.0, 43.0, 52.0, 57.0 ];
      let humidityExtMockData = [ 20.0, 22.0, 18.0, 27.0, 23.0 ];

      let temperatureId = 'temperature-' + monitor.id.toString();
      let humidityId = 'humidity-' + monitor.id.toString();

      monitor.temperatureChart = new Chart(temperatureId, {
        type: 'line',

        data: {
          labels: timeMockDataFmt,
          datasets: [
            {
              label: 'Interior',
              data: temperatureIntMockData,
              borderColor: "#c10767",
              pointBackgroundColor: "#c10767",
              pointHitRadius: 6,
              fill: false
            },
            {
              label: 'Exterior',
              data: temperatureExtMockData,
              borderColor: "#0767c1",
              pointBackgroundColor: "#0767c1",
              pointHitRadius: 6,
              fill: false
            }
          ]
        },

        options: {
          legend: {
            display: true
          },
          scales: {
            xAxes: [{
              display: false
            }],
            yAxes: [{
              display: true
            }],
          }
        }
      });

      monitor.humidityChart = new Chart(humidityId, {
        type: 'line',

        data: {
          labels: timeMockDataFmt,
          datasets: [
            {
              label: 'Interior',
              data: humidityIntMockData,
              borderColor: "#c10767",
              pointBackgroundColor: "#c10767",
              pointHitRadius: 6,
              fill: false
            },
            {
              label: 'Exterior',
              data: humidityExtMockData,
              borderColor: "#0767c1",
              pointBackgroundColor: "#0767c1",
              pointHitRadius: 6,
              fill: false
            }
          ]
        },

        options: {
          legend: {
            display: true
          },

          scales: {
            xAxes: [{
              display: false
            }],
            yAxes: [{
              display: true
            }],
          },
        }
      });
    });
  }

  ngOnInit(): void {
    this.initCharts();
  }

  monitors: Monitor[] = [ new Monitor(0), new Monitor(1) ];

  selectedMonitor: Monitor;
  onSelect(monitor: Monitor): void {
    this.selectedMonitor = monitor
  }

}
