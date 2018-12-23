import { Component, OnInit } from '@angular/core';
import { VersionService } from './version.service';
import { Version } from './version';
import { startWith, switchMap } from 'rxjs/operators';
import { interval } from 'rxjs';

@Component({
  selector: 'app-version',
  templateUrl: './version.component.html',
  styleUrls: ['./version.component.scss']
})
export class VersionComponent implements OnInit {
  private fullVersion: string;
  private foundVersion: boolean;

  constructor(private versionService: VersionService) { }

  ngOnInit() {
    this.update();

    interval(30000)
      .pipe(
        startWith(0),
        switchMap(() => this.versionService.get())
      )
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
