import { Component, OnInit } from '@angular/core';

import { MonitorType, Monitor, ClimateMonitor } from './monitor';
import { interval } from 'rxjs';
import { startWith, switchMap } from 'rxjs/operators';
import { ClimateService } from './climate/climate.service';
import { ClimateData } from './climate/climate';
import { MatDialogRef, MatDialog } from '@angular/material';

@Component({
  selector: 'app-monitor',
  templateUrl: './monitor.component.html',
  styleUrls: ['./monitor.component.css']
})
export class MonitorComponent implements OnInit {
  private lastUpdate = 0;

  private monitors: Monitor[] = [
    new ClimateMonitor('climate-interior', 'Interior climate'),
    new ClimateMonitor('climate-exterior', 'Exterior climate')
  ];

  constructor(private climateService: ClimateService, public dialog: MatDialog) {}

  ngOnInit(): void {
    this.initCharts().then(() =>
      interval(30000).pipe(
        startWith(0),
        switchMap(() => this.climateService.getData(this.lastUpdate))
      )
      .subscribe(data => this.updateClimate(data))
    );
  }

  private async delay(ms: number) {
    await new Promise(resolve => setTimeout(() => resolve(), ms))
      .then(() => console.log('fired'));
  }

  private async initCharts(): Promise<void> {
    // TODO: How can we do this cleanly?
    while (!document.getElementById('climate-interior')) {
      await this.delay(50);
    }

    this.monitors.forEach(monitor => monitor.init());
  }

  private updateClimate(data?: ClimateData): void {
    if (!data) {
      return;
    }

    const newData = [data.interior, data.exterior];

    for (let i = 0; i < 2; ++i) {
      const newTimestamps = newData[i].timestamps.filter(timestamp => timestamp > this.lastUpdate);
      const newSamples = newData[i].samples.slice(newData[i].samples.length - newTimestamps.length);

      for (const newTimestamp of newTimestamps) {
        this.monitors[i].data.timestamps.push(newTimestamp);
      }

      for (const newSample of newSamples) {
        this.monitors[i].data.datasets[0].push(newSample.temperature);
        this.monitors[i].data.datasets[1].push(newSample.humidity);
      }
    }

    // Update last timestamp
    if (data.interior.timestamps.length > 0) {
      this.lastUpdate = data.interior.timestamps[data.interior.timestamps.length - 1];
    }

    // Update charts
    this.updateCharts();
  }

  private updateCharts(): void {
    this.monitors.forEach(monitor => monitor.updateData());
  }

  private confirmDataReset(): void {
    const dialogRef = this.dialog.open(ConfirmResetDialogComponent);

    dialogRef.afterClosed().subscribe(result => {
      if (result) { this.resetData(); }
    });
  }

  private resetData(): void {
    console.log('Monitor.resetData() called, clearing climate data');

    this.climateService.deleteData().subscribe();

    this.monitors.forEach(monitor => monitor.resetData());

    this.updateCharts();
  }
}

@Component({
  selector: 'app-confirm-reset-dialog',
  templateUrl: 'confirm-reset-dialog.component.html'
})
export class ConfirmResetDialogComponent {
  constructor(public dialogRef: MatDialogRef<ConfirmResetDialogComponent>) {}

  onNoClick(): void {
    this.dialogRef.close();
  }
}
