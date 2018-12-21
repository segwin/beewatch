import { Injectable } from '@angular/core';
import { HttpClient, HttpHeaders, HttpParams } from '@angular/common/http';
import { Observable } from 'rxjs';

@Injectable({
  providedIn: 'root'
})
export class RestService {
  private endpointUri: string;
  private headers = new HttpHeaders({
    'Content-Type':  'application/json'
  });

  constructor(private http: HttpClient) {
    this.endpointUri = '/api/v1/';
  }

  public get<T>(uri: string, params?: HttpParams): Observable<T> {
    const options = { headers: this.headers, params: params };
    return this.http.get<T>(this.endpointUri + uri, options);
  }

  public post<T>(uri: string, data: Object): Observable<T> {
    const options = { headers: this.headers };
    return this.http.post<T>(this.endpointUri + uri, JSON.stringify(data), options);
  }
}
