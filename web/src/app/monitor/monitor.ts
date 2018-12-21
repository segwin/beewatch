import { Chart } from 'chart.js';
import { formatDate } from '@angular/common';

export abstract class Monitor {
    id: string;
    title: string;

    chart: Chart = null;

    constructor(id: string, title: string) {
        this.id = id;
        this.title = title;
    }

    abstract async init(): Promise<void>;

    private formatTime(timestamp: number): string {
        return formatDate(new Date(timestamp * 1000), 'short', 'en-CA');
    }

    validateData(timestamps: number[], datasetSamples: number[][]): boolean {
        if (datasetSamples.length !== this.chart.data.datasets.length) {
            console.error('Invalid request on monitor ' + this.id + ': mismatch between given data array & chart datasets length ' +
                '(' + datasetSamples.length.toString() + ' vs. ' + this.chart.data.datasets.length.toString() + ')');

            return false;
        }

        datasetSamples.forEach((data, index) => {
            if (timestamps.length !== data.length) {
                console.error('Invalid request on monitor ' + this.id + ': mismatch between timestamps & data array lengths ' +
                    '(' + timestamps.length.toString() + ' vs. ' + data.length.toString() + ')');

                return false;
            }
        });

        return true;
    }

    async setData(timestamps: number[], datasetSamples: number[][]): Promise<void> {
        this.validateData(timestamps, datasetSamples);

        // Clear existing data
        this.chart.data.labels = [];
        datasetSamples.forEach((data, i) => this.chart.data.datasets[i].data = []);

        // Add new data & update chart
        this.addData(timestamps, datasetSamples);
    }

    async addData(timestamps: number[], datasetSamples: number[][]): Promise<void> {
        this.validateData(timestamps, datasetSamples);

        // Add timestamps
        timestamps.forEach(timestamp => this.chart.data.labels.push(this.formatTime(timestamp)));

        // Add data samples
        datasetSamples.forEach((data, i) =>
            data.forEach(sample => this.chart.data.datasets[i].data.push(sample))
        );

        // Update chart
        this.chart.update();
    }
}

export class ClimateMonitor extends Monitor {
    constructor(id: string, title: string) {
        super(id, title);
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
