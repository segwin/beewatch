import { Injectable } from '@angular/core';
import { HttpClient, HttpHeaders, HttpParams } from '@angular/common/http';
import { Observable } from 'rxjs';

@Injectable({
  providedIn: 'root'
})
export class RestService {
  private headers = new HttpHeaders({
    'Content-Type':  'application/json'
  });

  constructor(private http: HttpClient) {
  }

  getEndpointUri(): string {
    return window.location.origin + '/api/v1/';
  }

  public get<T>(relativeUri: string, params?: HttpParams): Observable<T> {
    const uri = this.getEndpointUri() + relativeUri + '?' + params.toString();
    return this.http.get<T>(uri, { headers: this.headers});
  }

  public put<T>(relativeUri: string, data: Object): Observable<T> {
    const uri = this.getEndpointUri() + relativeUri;
    return this.http.put<T>(uri, JSON.stringify(data), { headers: this.headers});
  }
}
