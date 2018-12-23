import { Chart } from 'chart.js';
import { formatDate } from '@angular/common';

export interface MonitorData {
    timestamps: number[];
    datasets: number[][];
}

export abstract class Monitor {
    id: string;
    title: string;

    data: MonitorData;

    chart: Chart = null;

    constructor(id: string, title: string) {
        this.id = id;
        this.title = title;

        this.data = { timestamps: [], datasets: [] };
    }

    // Interface methods
    abstract getType(): string;
    abstract async init(): Promise<void>;

    // Utility methods
    private formatTime(timestamp: number): string {
        return formatDate(new Date(timestamp * 1000), 'short', 'en-CA');
    }

    // Data manipulation methods
    private validateData(): boolean {
        if (this.data.datasets.length !== this.chart.data.datasets.length) {
            console.error('Invalid request on monitor ' + this.id + ': mismatch between given data array & chart datasets length ' +
                '(' + this.data.datasets.length.toString() + ' vs. ' + this.chart.data.datasets.length.toString() + ')');

            return false;
        }

        this.data.datasets.forEach((data, index) => {
            if (this.data.timestamps.length !== data.length) {
                console.error('Invalid request on monitor ' + this.id + ': mismatch between timestamps & data array lengths ' +
                    '(' + this.data.timestamps.length.toString() + ' vs. ' + data.length.toString() + ')');

                return false;
            }
        });

        return true;
    }

    async updateData(): Promise<void> {
        if (!this.validateData()) {
            return;
        }

        // Clear existing data
        this.chart.data.labels = [];
        this.data.datasets.forEach((dataset, i) => this.chart.data.datasets[i].data = []);

        // Add timestamps
        this.data.timestamps.forEach(timestamp => this.chart.data.labels.push(this.formatTime(timestamp)));

        // Add data samples
        this.data.datasets.forEach((dataset, i) =>
            dataset.forEach(sample => this.chart.data.datasets[i].data.push(sample))
        );

        // Update chart
        this.chart.update();
    }

    async resetData(): Promise<void> {
      this.data.timestamps.length = 0;
      this.data.datasets.forEach(samples => samples.length = 0);
    }
}

export class ClimateMonitor extends Monitor {
    constructor(id: string, title: string) {
        super(id, title);
    }

    // Interface methods
    getType(): string {
        return 'climate';
    }

    async init(): Promise<void> {
        this.chart = new Chart(this.id, {
            type: 'line',

            data: {
                labels: [],
                datasets: [
                    {
                        label: 'Temperature (°C)',
                        yAxisID: 'temperature',
                        data: [],
                        fill: false,
                        borderColor: 'rgba(235,0,78,0.9)',
                        backgroundColor: 'rgba(235,0,78,0.25)',
                        pointHitRadius: 6,
                    },
                    {
                        data: [],
                        label: 'Humidity (%)',
                        yAxisID: 'humidity',
                        fill: true,
                        borderColor: 'rgba(0,156,235,0.9)',
                        backgroundColor: 'rgba(0,156,235,0.25)',
                        pointHitRadius: 6,
                    }
                ]
            },

            options: {
                legend: {
                    display: false
                },

                scales: {
                    xAxes: [{
                        display: true,
                    }],
                    yAxes: [
                        {
                            id: 'temperature',
                            type: 'linear',
                            position: 'left',
                            scaleLabel: {
                                display: true,
                                labelString: 'Temperature (°C)',
                                fontColor: 'rgba(235,0,78,1)',
                                fontStyle: 'bold'
                            },
                            ticks: {
                                suggestedMin: 0,
                                suggestedMax: 10,
                                fontColor: 'rgba(235,0,78,1)'
                            }
                        },
                        {
                            id: 'humidity',
                            type: 'linear',
                            position: 'right',
                            scaleLabel: {
                                display: true,
                                labelString: 'Humidity (%)',
                                fontColor: 'rgba(0,156,235,1)',
                                fontStyle: 'bold'
                            },
                            ticks: {
                                min: 0,
                                max: 100,
                                fontColor: 'rgba(0,156,235,1)'
                            }
                        }
                    ],
                },
            }
        });
    }
}
