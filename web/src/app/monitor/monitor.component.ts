import { Component, OnInit } from '@angular/core';
import { Monitor } from './monitor';

@Component({
  selector: 'app-monitor',
  templateUrl: './monitor.component.html',
  styleUrls: ['./monitor.component.css']
})

export class MonitorComponent implements OnInit {

  constructor() {}

  ngOnInit() {
  }

  monitors: Monitor[] = [ { id: 0 }, { id: 1 } ];

  selectedMonitor: Monitor;
  onSelect(monitor: Monitor): void {
    this.selectedMonitor = monitor
  }

}
