import { Injectable } from '@angular/core';
import { Observable } from 'rxjs';
import { catchError, retry } from 'rxjs/operators';

import { ClimateData } from './climate';
import { RestService } from 'src/app/rest.service';
import { HttpParams } from '@angular/common/http';

@Injectable({
  providedIn: 'root'
})
export class ClimateService {
  constructor(private rest: RestService) { }

  getData(since: number): Observable<ClimateData> {
    const requestParams = new HttpParams().set('since', (since + 1).toString());

    return this.rest.get<ClimateData>('data/climate', requestParams)
                    .pipe( retry(3), catchError(this.rest.handleError<ClimateData>('ClimateService.getData')) );
  }

  deleteData(): Observable<void> {
    return this.rest.delete<void>('data/climate')
                    .pipe( retry(3), catchError(this.rest.handleError<void>('ClimateService.getData')) );
  }
}
