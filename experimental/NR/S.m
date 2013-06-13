function [P Q] = S(bus, G, B, M, t)
   Mc = M .* cos(t);
   Ms = M .* sin(t);
   QPV = Mc(bus.i1PV:bus.i2PV) .* ...
         (-G(bus.i1PV:bus.i2PV,:)*Ms - B(bus.i1PV:bus.i2PV,:)*Mc) ...
       + Ms(bus.i1PV:bus.i2PV) .* ...
         ( G(bus.i1PV:bus.i2PV,:)*Mc - B(bus.i1PV:bus.i2PV,:)*Ms);
   PSL = Mc(bus.i1SL:bus.i2SL) .* ...
         ( G(bus.i1SL:bus.i2SL,:)*Mc - B(bus.i1SL:bus.i2SL,:)*Ms) ...
       + Ms(bus.i1SL:bus.i2SL) .* ...
         ( G(bus.i1SL:bus.i2SL,:)*Ms + B(bus.i1SL:bus.i2SL,:)*Mc);
   QSL = Mc(bus.i1SL:bus.i2SL) .* ...
         (-G(bus.i1SL:bus.i2SL,:)*Ms - B(bus.i1SL:bus.i2SL,:)*Mc) ...
       + Ms(bus.i1SL:bus.i2SL) .* ...
         ( G(bus.i1SL:bus.i2SL,:)*Mc - B(bus.i1SL:bus.i2SL,:)*Ms);
   P = [bus.P(bus.i1PQ:bus.i2PV);PSL];
   Q = [bus.Q(bus.i1PQ:bus.i2PQ);QPV;QSL];
end