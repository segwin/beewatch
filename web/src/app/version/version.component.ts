import { Component, OnInit } from '@angular/core';
import { VersionService } from './version.service';
import { Version } from './version';

@Component({
  selector: 'app-version',
  templateUrl: './version.component.html',
  styleUrls: ['./version.component.scss']
})
export class VersionComponent implements OnInit {
  private fullVersion: string;
  private foundVersion: boolean;

  constructor(private version: VersionService) { }

  ngOnInit() {
    this.update();

    this.version.get()
                .subscribe(version => this.update(version));
  }

  private update(version?: Version): void {
    if (version) {
      this.fullVersion = version.version;
      this.foundVersion = true;
    } else {
      this.fullVersion = 'Connecting to app...';
      this.foundVersion = false;
    }
  }

}
