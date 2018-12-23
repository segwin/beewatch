import { BrowserModule } from '@angular/platform-browser';
import { NgModule } from '@angular/core';

import { FormsModule } from '@angular/forms';
import { BrowserAnimationsModule } from '@angular/platform-browser/animations';
import { MatButtonModule, MatCardModule, MatDialogModule, MatDividerModule,
         MatGridListModule, MatIconModule, MatInputModule, MatProgressBarModule,
         MatToolbarModule } from '@angular/material';

import { AppComponent } from './app.component';
import { HttpClientModule } from '@angular/common/http';
import { MonitorComponent, ConfirmResetDialogComponent } from './monitor/monitor.component';
import { VersionComponent } from './version/version.component';

@NgModule({
  declarations: [
    AppComponent,
    MonitorComponent, ConfirmResetDialogComponent,
    VersionComponent,
  ],
  entryComponents: [
    ConfirmResetDialogComponent
  ],
  imports: [
    BrowserModule,
    FormsModule,
    BrowserAnimationsModule,
    MatButtonModule, MatCardModule, MatDialogModule, MatDividerModule,
    MatGridListModule, MatIconModule, MatInputModule, MatProgressBarModule, MatToolbarModule,
    HttpClientModule,
  ],
  providers: [],
  bootstrap: [AppComponent]
})
export class AppModule { }
