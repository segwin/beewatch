import { BrowserModule } from '@angular/platform-browser';
import { NgModule } from '@angular/core';

import { FormsModule } from '@angular/forms';
import { BrowserAnimationsModule } from '@angular/platform-browser/animations';
import { MatButtonModule, MatCardModule, MatGridListModule, MatIconModule, MatInputModule, MatToolbarModule } from '@angular/material';

import { AppComponent } from './app.component';
import { HttpClientModule } from '@angular/common/http';
import { MonitorComponent } from './monitor/monitor.component';

@NgModule({
  declarations: [
    AppComponent,
    MonitorComponent,
  ],
  imports: [
    BrowserModule,
    FormsModule,
    BrowserAnimationsModule,
    MatButtonModule, MatCardModule, MatGridListModule, MatIconModule, MatInputModule, MatToolbarModule,
    HttpClientModule,
  ],
  providers: [],
  bootstrap: [AppComponent]
})
export class AppModule { }
