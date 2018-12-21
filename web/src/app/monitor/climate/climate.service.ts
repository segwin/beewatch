import { Injectable } from '@angular/core';
import { HttpParams } from '@angular/common/http';
import { Observable, of } from 'rxjs';
import { catchError } from 'rxjs/operators';

import { ClimateData } from './climate';
import { RestService } from 'src/app/rest.service';

const MOCK_DATA: ClimateData = {
  interior: {
    timestamps: [1545103255, 1545103160, 1545103165, 1545103170, 1545103175],
    samples: [
      { temperature: 2.5, humidity: 62.5 },
      { temperature: 1.5, humidity: 50.5 },
      { temperature: -1.5, humidity: 47.5 },
      { temperature: 3.5, humidity: 70.5 },
      { temperature: 5.0, humidity: 80.5 },
    ]
  },

  exterior: {
    timestamps: [1545103255, 1545103160, 1545103165, 1545103170, 1545103175],
    samples: [
      { temperature: 2.5, humidity: 62.5 },
      { temperature: 1.5, humidity: 50.5 },
      { temperature: -1.5, humidity: 47.5 },
      { temperature: 3.5, humidity: 70.5 },
      { temperature: 5.0, humidity: 80.5 },
    ]
  }
};

@Injectable({
  providedIn: 'root'
})
export class ClimateService {
  constructor(private rest: RestService) { }

  dateToTimestamp(date: Date): number {
    return date.getTime() / 1000;
  }

  getData(since: number): Observable<ClimateData> {
    const requestParams = new HttpParams();
    requestParams.append('since', (since + 1).toString());

    return this.rest.get<ClimateData>('data/climate', requestParams)
                    .pipe( catchError(this.handleError<ClimateData>('ClimateService.getData')) );
  }

  private handleError<T>(operation = 'operation', result?: T) {
    return (error: any): Observable<T> => {
      console.log(`${operation} failed: ${error.message}`);

      // Let the app keep running by returning an empty result.
      return of(result as T);
    };
  }
}
