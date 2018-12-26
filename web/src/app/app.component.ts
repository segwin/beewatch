import { Component } from '@angular/core';

export const pages: string[] = [ 'Monitor', 'Settings' ];

@Component({
  selector: 'app-root',
  templateUrl: './app.component.html',
  styleUrls: ['./app.component.scss']
})
export class AppComponent {
  name = 'Your Bee Monitor';
  editingName = false;
}
